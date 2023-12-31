//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

//# {{
#include "preprocessor.hpp"
#if !TOML_IMPLEMENTATION
#error This is an implementation-only header.
#endif
//# }}

#include "node.hpp"
#include "node_view.hpp"
#include "at_path.hpp"
#include "table.hpp"
#include "array.hpp"
#include "value.hpp"
#include "header_start.hpp"

TOML_NAMESPACE_START
{
	TOML_EXTERNAL_LINKAGE
	node::node() noexcept = default;

	TOML_EXTERNAL_LINKAGE
	node::~node() noexcept = default;

	TOML_EXTERNAL_LINKAGE
	node::node(node && other) noexcept //
		: source_{ std::exchange(other.source_, {}) }
	{}

	TOML_EXTERNAL_LINKAGE
	node::node(const node& /*other*/) noexcept
	{
		// does not copy source information - this is not an error
		//
		// see https://github.com/marzer/tomlplusplus/issues/49#issuecomment-665089577
	}

	TOML_EXTERNAL_LINKAGE
	node& node::operator=(const node& /*rhs*/) noexcept
	{
		// does not copy source information - this is not an error
		//
		// see https://github.com/marzer/tomlplusplus/issues/49#issuecomment-665089577

		source_ = {};
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	node& node::operator=(node&& rhs) noexcept
	{
		if (&rhs != this)
			source_ = std::exchange(rhs.source_, {});
		return *this;
	}

	TOML_EXTERNAL_LINKAGE
	node_view<node> node::at_path(std::string_view path) noexcept
	{
		return toml::at_path(*this, path);
	}

	TOML_EXTERNAL_LINKAGE
	node_view<const node> node::at_path(std::string_view path) const noexcept
	{
		return toml::at_path(*this, path);
	}

	TOML_EXTERNAL_LINKAGE
	node_view<node> node::at_path(const path& p) noexcept
	{
		return toml::at_path(*this, p);
	}

	TOML_EXTERNAL_LINKAGE
	node_view<const node> node::at_path(const path& p) const noexcept
	{
		return toml::at_path(*this, p);
	}

#if TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	node_view<node> node::at_path(std::wstring_view path)
	{
		return toml::at_path(*this, path);
	}

	TOML_EXTERNAL_LINKAGE
	node_view<const node> node::at_path(std::wstring_view path) const
	{
		return toml::at_path(*this, path);
	}

#endif // TOML_ENABLE_WINDOWS_COMPAT

	TOML_EXTERNAL_LINKAGE
	node_view<node> node::operator[](const path& p) noexcept
	{
		return toml::at_path(*this, p);
	}

	TOML_EXTERNAL_LINKAGE
	node_view<const node> node::operator[](const path& p) const noexcept
	{
		return toml::at_path(*this, p);
	}
}
TOML_NAMESPACE_END;

TOML_IMPL_NAMESPACE_START
{
	TOML_PURE_GETTER
	TOML_EXTERNAL_LINKAGE
	bool TOML_CALLCONV node_deep_equality(const node* lhs, const node* rhs) noexcept
	{
		// both same or both null
		if (lhs == rhs)
			return true;

		// lhs null != rhs null or different types
		if ((!lhs != !rhs) || lhs->type() != rhs->type())
			return false;

		return lhs->visit(
			[=](auto& l) noexcept
			{
				using concrete_type = remove_cvref<decltype(l)>;

				return l == *(rhs->as<concrete_type>());
			});
	}
}
TOML_IMPL_NAMESPACE_END;

#include "header_end.hpp"
