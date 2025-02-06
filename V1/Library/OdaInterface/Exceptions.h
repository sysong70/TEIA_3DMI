#pragma once

//--------------------------------------------------------------------------------------------------

class ExceptionBase
{
};

// Cancel command

class ExpCancel : public ExceptionBase
{
};

// Use default value

class ExpEmptyInput : public ExceptionBase
{
};

// Invalid

class ExpError : public ExceptionBase
{
};

// Switch process

class ExpKeyword : public ExceptionBase
{
};

// Point or Real?

class ExpOtherInput : public ExceptionBase
{
};
