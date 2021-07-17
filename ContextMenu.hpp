#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <vector>
#include <windef.h>
#include <ShObjIdl_core.h>


class ContextMenuItem {
	std::wstring text;
	std::wstring verb;
	std::vector<ContextMenuItem> children;
	std::function<HRESULT(const std::vector<std::filesystem::path>&)> invoke;
	// Remembered from last insert
	uint32_t ownPosition;
	uint16_t ownCmd;
public:

	ContextMenuItem() : invoke([](const std::vector<std::filesystem::path>&) { return E_NOTIMPL; }) {};
	ContextMenuItem(std::wstring text, std::wstring verb, std::function<HRESULT(const std::vector<std::filesystem::path>&)> invoke) : text(text), verb(verb), invoke(invoke) {};

	void AddChild(ContextMenuItem child) {
		children.emplace_back(std::move(child));
	}

	// returns how many items were inserted including submenus
	uint16_t InsertIntoMenu(HMENU parentMenu, uint32_t& position, uint32_t& id);
	std::wstring GetVerb(uint32_t cmd) const;
	HRESULT Invoke(const std::vector<std::filesystem::path>& files) const { return invoke(files); }
	HRESULT InvokeByVerb(std::wstring_view verb, const std::vector<std::filesystem::path>& files) const {
		if (verb == this->verb)
			return invoke(files);

		for (const auto& it : children)
			if (SUCCEEDED(it.InvokeByVerb(verb, files)))
				return S_OK;
		return E_INVALIDARG;
	}

	HRESULT InvokeByCmd(uint16_t cmd, const std::vector<std::filesystem::path>& files) const {
		if (cmd == ownCmd)
			return invoke(files);

		for (const auto& it : children)
			if (SUCCEEDED(it.InvokeByCmd(cmd, files)))
				return S_OK;
		return E_INVALIDARG;
	}

	bool HasChildren() const {
		return !children.empty();
	}
};

