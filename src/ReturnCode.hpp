#ifndef RETURN_CODE_HPP
#define RETURN_CODE_HPP

enum ReturnCode {
	FAILED,     /// task failed completely and is possibly an error, exit might be necessary
	INCOMPLETE, /// task is incomplete and is possibly at warning level, exit not necessary
	SUCCESS,    /// task is successfully completed
};

#endif//RETURN_CODE_HPP
