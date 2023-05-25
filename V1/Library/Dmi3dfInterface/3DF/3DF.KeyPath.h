#pragma once

#include "3DF.h"
#include "3DF.Key.h"
#include "3DF.Math.h"

OPEN_3DF_NAMESPACE

class KeyPath : public Object
{
public:
	KeyPath();
	KeyPath(KeyArray const & cInPath);
	KeyPath(size_t nInPathCount, Key const pInPath[]);
	KeyPath(size_t nInPathCount, HC_KEY const pInPath[]);
	KeyPath(KeyPath const & cInThat);
	KeyPath(char chKeyPath[]);

	void Set(KeyPath const & cInThat);

	KeyPath & operator=(KeyPath const & cInThat);
	KeyPath & operator=(KeyArray const & cInPath);

	KeyPath & SetKeys(KeyArray const & cInKeys);
	KeyPath & SetKeys(size_t nInKeyCount, HC_KEY const pInKeys[]);

	KeyPath & UnsetKeys();

	bool ShowKeys(KeyArray & cOutKeys) const;

	bool ConvertCoordinate(Coordinate::Space eInSpace, Point const & cInpoint, Coordinate::Space eInOutputSpace, Point & cOutPoint) const;
	bool ConvertCoordinate(Coordinate::Space eInSpace, PointArray const & aInPoints, Coordinate::Space eInOutputSpace, PointArray & aOutPoints) const;

	// Shows the net selectability attribute settings along this key path.
	bool ShowNetSelectability(SelectabilityKit & cOutKit) const;

	// Shows the net modelling matrix attribute settings along this key path.
	bool ShowNetModellingMatrix(MatrixKit & cOutKit) const;

	size_t Size() const;

	virtual bool Empty() const;

	Key & At(size_t nInIndex);
	Key const & At(size_t nInIndex) const;

	void Insert(size_t nInIndex, Key const & cInItem);
	void Insert(size_t nInIndex, HC_KEY nInKey);

	void Remove(Key const & cInItem);
	void Remove(size_t nInIndex);

	// Creates and returns a new KeyPath object, with the keys organized in the reverse order.
	KeyPath Reverse() const;

	Key & Front();
	Key const & Front() const;

	Key & Back();
	Key const & Back() const;

	Key PopFront();
	Key PopBack();

	KeyPath & PushFront(Key const & cInKey);
	KeyPath & PushBack(Key const & cInKey);
};

CLOSE_3DF_NAMESPACE