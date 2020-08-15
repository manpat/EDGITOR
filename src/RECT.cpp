#include "RECT.h"

#include <algorithm>
#include <limits>

// manpat: really not a fan of this - this should be cmakes job :(
#ifdef __APPLE__
#include <SDL2/SDL_rect.h>
#else
#include <SDL_rect.h>
#endif

/*static*/ RECT RECT::empty()
{
	using limits = std::numeric_limits<int>;

	return RECT {
		limits::max(), limits::max(),
		limits::min(), limits::min(),
	};
}

/*static*/ RECT RECT::from_wh(int w, int h)
{
	return {0, 0, w, h};
}

/*static*/ RECT RECT::from_xywh(int x, int y, int w, int h)
{
	return {x, y, x+w, y+h};
}


bool operator==(RECT const& lhs, RECT const& rhs)
{
	return lhs.left == rhs.left
		&& lhs.top == rhs.top
		&& lhs.right == rhs.right
		&& lhs.bottom == rhs.bottom;
}

bool operator!=(RECT const& lhs, RECT const& rhs)
{
	return !(lhs == rhs);
}


bool RECT::is_empty() const
{
	return this->left >= this->right
		|| this->top >= this->bottom;
}

int RECT::width() const
{
	return std::max(this->right - this->left, 0);
}

int RECT::height() const
{
	return std::max(this->bottom - this->top, 0);
}


RECT RECT::include_region(RECT other) const
{
	return {
		std::min(this->left, other.left),
		std::min(this->top, other.top),
		std::max(this->right, other.right),
		std::max(this->bottom, other.bottom),
	};
}

RECT RECT::include_point(int x, int y) const
{
	return {
		std::min(this->left, x),
		std::min(this->top, y),
		std::max(this->right, x+1),
		std::max(this->bottom, y+1),
	};
}

RECT RECT::clip_to(RECT boundary) const
{
	return {
		std::clamp(this->left, boundary.left, boundary.right),
		std::clamp(this->top, boundary.top, boundary.bottom),
		std::clamp(this->right, boundary.left, boundary.right),
		std::clamp(this->bottom, boundary.top, boundary.bottom),
	};
}

SDL_Rect RECT::to_sdl() const
{
	return {
		this->left, this->top,
		this->width(), this->height(),
	};
}



RECT_ITERATOR& RECT_ITERATOR::operator++()
{
	this->xy.first++;
	if (this->xy.first >= this->rect.right) {
		this->xy.first = this->rect.left;
		this->xy.second++;
	}

	return *this;
}

RECT_ITERATOR RECT_ITERATOR::operator++(int)
{
	auto copy = *this;
	operator++();
	return copy;
}

std::pair<int, int> const& RECT_ITERATOR::operator*() const
{
	return this->xy;
}

std::pair<int, int> const* RECT_ITERATOR::operator->() const
{
	return &this->xy;
}

bool operator==(RECT_ITERATOR const& lhs, RECT_ITERATOR const& rhs)
{
	return lhs.xy == rhs.xy
		&& lhs.rect == rhs.rect;
}

bool operator!=(RECT_ITERATOR const& lhs, RECT_ITERATOR const& rhs)
{
	return !(lhs == rhs);
}


RECT_ITERATOR begin(RECT rect)
{
	return {rect, {rect.left, rect.top}};
}

RECT_ITERATOR end(RECT rect)
{
	return {rect, {rect.left, rect.bottom}};
}
