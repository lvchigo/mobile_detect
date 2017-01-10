/*
 * =====================================================================================
 *
 *       filename:  string_operator.h
 *
 *    description:  string operator functions
 *
 *        version:  1.0
 *        created:  2014-10-28
 *       revision:  none
 *       compiler:  g++
 *
 *         author:  tangyuan
 *        company:  itugo.com
 *
 *      copyright:  2014 itugo Inc. All Rights Reserved.
 *      
 * =====================================================================================
 */

#ifndef STRING_OPERATOR_H
#define STRING_OPERATOR_H

#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <ctime>
#include <sys/time.h>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <assert.h>

namespace geekeyelab {

	const char SLASH_CHR     = '/';        /* slash char in string */
	const char DOT_CHR       = '.';        /* dot char in string */
        
	static inline void string_replace(std::string & strBig, const std::string & strsrc, const std::string &strdst)
	{
		std::string::size_type pos=0;
		std::string::size_type srclen=strsrc.size();
		std::string::size_type dstlen=strdst.size();
		while( (pos=strBig.find(strsrc, pos)) != std::string::npos)
	{
		strBig.replace(pos, srclen, strdst);
		pos += dstlen;
	}
	}

	/** local (static) compare function for the qsort() call */
	static inline int str_compare( const void *arg1, const void *arg2 )
	{
		return strcmp ( ( * ( std::string* ) arg1 ).c_str (), ( * ( std::string* ) arg2 ).c_str () );
	}

	/**
	 * @brief split a string by delim
	 *
	 * @param str string to be splited
	 * @param c delimiter, const char*, just like " .,/", white space, dot, comma, splash
	 *
	 * @return a string vector saved all the splited world
	 */
	static inline std::vector<std::string> split(const std::string& str, const char* c)
    {
			char *cstr, *p;
			std::vector<std::string> res;
			cstr = new char[str.size()+1];
			strcpy(cstr,str.c_str());
			p = strtok(cstr,c);
			while(p!=NULL)
			{
				res.push_back(p);
				p = strtok(NULL,c);
			}
			return res;
	}

	/* string to number */
	template<typename T>
	T string_to_number(const std::string &str)
	{
		T val;
		std::stringstream stream(str);
		stream >> val;
		
		return val;
	}

	/* number to string */
	template<typename T>
	std::string  number_to_string(const T &val)
	{
		
		std::ostringstream stream;
		stream << val;

		return stream.str();
	}

	

	// read real number from stream, such as int float double //
	template <typename T>
	static inline T read_real(std::istream &is)
	{
		T var;
		is.read( reinterpret_cast<char *>(&var), sizeof(T));
		return var;
	}

	// write real number to stream, such as int float double //
	template <typename T>
	static inline void write_real(std::ostream &os, T var)
	{
		os.write( reinterpret_cast<char *>(&var), sizeof(T));
	}

	// read real number array from stream, such as int* float* double* //
	template <typename T>
	static inline void read_real(std::istream &is, T *var, uint64_t len)
	{
		uint64_t sz = sizeof(T) * len;
	    is.read( reinterpret_cast<char *>(var), sz);
	}

	// write real number array to stream, such as int* float* double* //
	template <typename T>
	static inline void write_real(std::ostream &os, T *var, uint64_t len)
	{
		uint64_t sz = sizeof(T) * len;
		os.write( reinterpret_cast<char *>(var), sz);
	}

	// read string //
	static inline void read_string(std::istream &is, std::string &str)
	{
		size_t sz = read_real<uint32_t>( is );
		str.resize(sz);
		if(sz)
		{
			is.read(&str.at(0), sz);
		}
	}
	
	// write string //
	static inline void write_string(std::ostream &os, const std::string &str)
	{
		write_real<uint32_t>(os, str.size());
		if(str.size())
		{
			os.write(&str[0], str.size());
		}
	}
/*=================================================*/
	/**
	 * @brief   program runtimer
	 */
    template<class T = double>
		class RunTimer
		{
		public:
		
		/*constructor*/
		RunTimer()
		:rt_start(0), rt_end(0)
		{}
		
		/*destructor*/
		~RunTimer()
		{}

		void start()
		{
			//rt_start = static_cast<T>(clock()) / CLOCKS_PER_SEC;
			//rt_end   = rt_start;
			gettimeofday(&tv_start, NULL);
			gettimeofday(&tv_end, NULL);
		}

		void end()
		{
			//rt_end = static_cast<T>(clock()) / CLOCKS_PER_SEC;
			gettimeofday(&tv_end, NULL);
		}

		T time()
		{
			//return (rt_end - rt_start);
			return static_cast<T>(timediff(&tv_start, &tv_end) / 1000000.0);
		}
		private:
		T rt_start;
		T rt_end;
		timeval tv_start;
		timeval tv_end;

		long long int timediff(timeval *start, timeval *end)
		{
			return (
					(end->tv_sec * 1000000 + end->tv_usec) -
					(start->tv_sec * 1000000 + start->tv_usec)
					);
		}
		};/*RunTimer*/


} // geekeyelab	

#endif
