#pragma once

#include <utility>
#include <iterator>

struct SDL_Rect;


struct [[nodiscard]] RECT {
	int left;
	int top;
	int right;
	int bottom;

	static RECT empty();
	static RECT from_wh(int w, int h);
	static RECT from_xywh(int x, int y, int w, int h);

	friend bool operator==(RECT const&, RECT const&);
	friend bool operator!=(RECT const&, RECT const&);

	bool is_empty() const;
	int width() const;
	int height() const;

	// expand to encompass the passed RECT or point
	RECT include_region(RECT) const;
	RECT include_point(int x, int y) const;

	// shrink to fit within boundary
	RECT clip_to(RECT boundary) const;

	SDL_Rect to_sdl() const;
};


/// Helpers to allow RECTs to be iterated over

struct [[nodiscard]] RECT_ITERATOR {
	RECT rect;
	std::pair<int, int> xy; // using a pair here bc C++

	RECT_ITERATOR& operator++();
	RECT_ITERATOR operator++(int);

	std::pair<int, int> const& operator*() const;
	std::pair<int, int> const* operator->() const;

	friend bool operator==(RECT_ITERATOR const&, RECT_ITERATOR const&);
	friend bool operator!=(RECT_ITERATOR const&, RECT_ITERATOR const&);
};

template<>
struct std::iterator_traits<RECT_ITERATOR> {
	using iterator_category = std::input_iterator_tag;

	using difference_type = void;
	using value_type = std::pair<int, int>;
	using pointer = value_type const*;
	using reference = value_type const&;
};

RECT_ITERATOR begin(RECT);
RECT_ITERATOR end(RECT);