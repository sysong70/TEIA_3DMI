#pragma once

#include "3DF.h"
#include "Key.h"
#include "Math.h"

OPEN_3DF_NAMESPACE

class API_3DF KeyPath : public Object
{
public:
	KeyPath();
	KeyPath(KeyArray const & cInPath);
	KeyPath(size_t nInPathCount, Key const pInPath[]);
	KeyPath(KeyPath const & cInThat);
	KeyPath(char chKeyPath[]);

	H3DF::Type ObjectType() const override { return H3DF::Type::KeyPath; };

	void Set(KeyPath const & cInThat);

	KeyPath & operator=(KeyPath const & cInThat);
	KeyPath & operator=(KeyArray const & cInPath);

	KeyPath & SetKeys(KeyArray const & cInKeys);

	KeyPath & UnsetKeys();

	bool ShowKeys(KeyArray & cOutKeys) const;

	bool ConvertCoordinate(Coordinate::Space eInSpace, Point const & cInpoint, Coordinate::Space eInOutputSpace, Point & cOutPoint) const;
	bool ConvertCoordinate(Coordinate::Space eInSpace, PointArray const & aInPoints, Coordinate::Space eInOutputSpace, PointArray & aOutPoints) const;

	// Shows the net modelling matrix attribute settings along this key path.
	bool ShowNetModellingMatrix(MatrixKit & cOutKit) const;

	size_t Size() const;

	virtual bool Empty() const;

	Key & At(size_t nInIndex);
	Key const & At(size_t nInIndex) const;

	void Insert(size_t nInIndex, Key const & cInItem);

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