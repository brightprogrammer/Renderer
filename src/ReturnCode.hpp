#ifndef RETURN_CODE_HPP
#define RETURN_CODE_HPP

enum class ReturnCode {
	Failed,     /// task failed completely and is possibly an error, exit might be necessary
	Incomplete, /// task is incomplete and is possibly at warning level, exit not necessary
	Success,    /// task is successfully completed
};

#endif//RETURN_CODE_HPP
