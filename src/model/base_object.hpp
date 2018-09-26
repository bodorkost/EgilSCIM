#include <utility>

/**
 * Copyright © 2017-2018  Max Wällstedt <>
 *
 * This file is part of SimpleSCIM.
 *
 * SimpleSCIM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SimpleSCIM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with SimpleSCIM.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SIMPLESCIM_USER_H
#define SIMPLESCIM_USER_H

#include <stddef.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <ostream>
#include <iostream>
#include <algorithm>

typedef std::pair<std::string, std::string> string_pair;

typedef std::string attrib_name;

typedef std::vector<std::string> string_vector;
typedef std::map<attrib_name, string_vector> attrib_map;

class base_object {
	attrib_map attributes{};

	mutable std::string identity;
	mutable std::string ss12000type;

	string_vector empty{};

	base_object() {
		attributes.emplace(std::make_pair("ss12000type", string_vector({"base"})));
	}

public:
	friend class cache_file;

	explicit base_object(const std::string &type) : ss12000type(type) {
		attributes.emplace(std::make_pair("ss12000type", string_vector({type})));
	}
	base_object(const attrib_name &attr, const string_vector &values) {
		add_attribute(attr, values);
	}

	base_object(const attrib_name &attr, string_vector &&values) {
		add_attribute(attr, std::move(values));
	}

	explicit base_object(attrib_map &&data) : attributes(std::move(data)) {}

	base_object(const base_object &other) {
		*this = other;
	}

	base_object(base_object &&other) noexcept {
		*this = std::move(other);
	}

	size_t size() {
		return attributes.size();
	}

	attrib_map::const_iterator begin() const {
		return attributes.begin();
	}

	attrib_map::const_iterator end() const {
		return attributes.end();
	}

	friend std::ostream &operator<<(std::ostream &os, const base_object &object);

	base_object &operator=(base_object &&other) noexcept {
		identity = std::move(other.identity);
		attributes = std::move(other.attributes);
		ss12000type = std::move(other.ss12000type);
		return *this;
	}

	base_object &operator=(const base_object &other) {
		identity = other.identity;
		attributes = other.attributes;
		ss12000type = other.ss12000type;
		return *this;
	}

	std::string get_uid(bool search = true) const;

	std::string getSS12000type() const {
		const string_vector list = get_values("ss12000type");
		if (!list.empty()) {
			return list.at(0);
		}
		std::cerr << "base_object::getSS12000type missing type" << std::endl;
		return "";
	}

	const string_vector &get_values(const std::string &attr) const {
		auto record = attributes.find(attr);
		if (record != attributes.end()) {
			return record->second;
		}
		return empty;
	}

	void sortAttribute(std::string a) {
		auto list = attributes.find(a);
		if (list != attributes.end())
			std::sort(list->second.begin(), list->second.end());
	}

	void append_values(const std::string &attr, const string_vector &values, bool unique = false) {
		auto iter = attributes.find(attr);
		if (iter != attributes.end()) {
			if (unique) {
				for (auto &&val: values) {
					if (std::find(std::begin(iter->second), std::end(iter->second), val) == std::end(iter->second))
						iter->second.emplace_back(val);
				}
			} else
				iter->second.insert(iter->second.end(), values.begin(), values.end());
		} else
			attributes.emplace(std::make_pair(attr, values));
	}

	void append_values(const std::string &attr, string_vector &&values, bool unique = false) {
		auto iter = attributes.find(attr);
		if (iter != attributes.end()) {
			if (unique) {
				for (auto &&val: values) {
					if (std::find(std::begin(iter->second), std::end(iter->second), val) == std::end(iter->second))
						iter->second.emplace_back(std::move(val));
				}
			} else
				iter->second.insert(iter->second.end(), values.begin(), values.end());
		} else
			attributes.emplace(std::make_pair(attr, std::move(values)));
	}

	void add_attribute(const std::string &attr, const string_vector &values) {
		auto iter = attributes.find(attr);

		if (iter != attributes.end()) {
			attributes.erase(attr);
		}
		attributes.emplace(std::make_pair(attr, values));
	}

	void add_attribute(const std::string &attr, string_vector &&values) {
		auto iter = attributes.find(attr);

		if (iter != attributes.end()) {
			attributes.erase(attr);
		}
		attributes.emplace(std::make_pair(attr, std::move(values)));
	}

	size_t number_of_attributes() const {
		return attributes.size();
	}

	bool has(const string_pair &what) {
		if (what.first.empty())
			return false;
		auto a = attributes.find(what.first);
		if (a != end() && (!a->second.empty())) {
			return *(a->second.begin()) == what.second;
		}
		return false;
	}

	/*
	 * testing if all in other is in this
	 * since one side may have a scim-id, other way doesn't work
	 * todo so: compare all except scim-id instead of all in other is in this
	 */
	bool operator==(const base_object &rhs) const {
		bool result = std::includes(rhs.attributes.begin(), rhs.attributes.end(), attributes.begin(), attributes.end());

		return result;
	}

};

#endif
