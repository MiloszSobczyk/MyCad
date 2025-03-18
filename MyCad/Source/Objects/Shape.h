#pragma once

class Shape
{
public:
	virtual ~Shape() = default;

	virtual void HandleInput() = 0;
	virtual void Render() = 0;
};