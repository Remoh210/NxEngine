#pragma once
#include <cstdint>
#include <string>

typedef int8_t int8;
typedef int32_t int32;
typedef uint32_t uint32;
typedef uint8_t uint8;
typedef std::string string;
typedef intptr_t intptr;
typedef uintptr_t uintptr;

#define NULL_COPY_AND_ASSIGN(T) \
	T(const T& other) {(void)other;} \
	void operator=(const T& other) { (void)other; 

#ifdef DEBUG
	#define AssertCheck assert
#else
	#define AssertCheck (void)
#endif


#define LOG_ERROR "Error"
#define LOG_WARNING "Warning"
#define LOG_TYPE_RENDERER "Renderer"
#define LOG_TYPE_IO "IO"
#define DEBUG_LOG(category, level, message, ...) \
	fprintf(stderr, "[%s] ", category); \
	fprintf(stderr, "[%s] (%s:%d): ", level, __FILE__, __LINE__); \
	fprintf(stderr, message, ##__VA_ARGS__); \
	fprintf(stderr, "\n")
#define DEBUG_LOG_TEMP(message, ...) DEBUG_LOG("TEMP", "TEMP", message, ##__VA_ARGS__)
#define DEBUG_LOG_TEMP2(message) DEBUG_LOG("TEMP", "TEMP", "%s", message)
#define DEBUG_PRINT(message, ...) \
	fprintf(stderr, message, ##__VA_ARGS__); \
	fprintf(stderr, "\n")
#define DEBUG_PRINT2(message) \
	fprintf(stderr, "[%s] ", message); \
	fprintf(stderr, "\n")
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))


inline int GetPercentOf(int percent, int number)
{
	return (int)((number * percent) / 100);
}

static inline float Randf(float minimum_number, float maximum_number)
{
	return minimum_number + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maximum_number - minimum_number)));
}

//Returns random float from 0.0 to 1.0
static inline float Randf()
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

static inline int Randi(int minimum_number, int maximum_number)
{
	return (rand() % (maximum_number + 1 - minimum_number)) + minimum_number;
}

