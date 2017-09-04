#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "interface.h"
#include "types.h"
#include "utils.h"

/* MACRO DEFINITION */

#define DIM 5U
#define NB_OF_STR 3
#define BUFF_SIZE 64U

#define CHECK_EXIT(ptr) \
  do {		        \
    check_exit(ptr);    \
  } while (0)

/* END MACRO */

/* TYPEDEFS */
typedef uint32_t array_t[2];

array_t dummy_dummy;

typedef enum
  {
    FMT_NULL,
    FMT_DEC,
    FMT_CHAR
  } format_t;

typedef enum
  {
    CODE_0=255,
    CODE_1=1,
    CODE_2
  } code_t;

struct vectStruct_t
{
    uint32_t id;
    uint32_t vector[DIM];
    bool_t init;
};

typedef struct vectStruct_t vectStruct_t;

struct infoStruct_t
{
    uint32_t key;
    vectStruct_t* vectorStruct;
};

typedef struct infoStruct_t infoStruct_t;

typedef enum
{
    VALID,
    INVALID
} valid_t;

struct buffStruct_t
{
  valid_t valid;
  uint32_t buff[BUFF_SIZE];
};

typedef struct buffStruct_t buffStruct_t;

struct dataStruct_t
{
  format_t format;
  char* id;
  buffStruct_t buffStruct;
};

typedef struct dataStruct_t dataStruct_t;

struct setStruct_t
{
  bool_t flag;
  dataStruct_t* data;
};

typedef struct setStruct_t setStruct_t;

struct setArrayStruct_t
{
  uint32_t setArraySize;
  setStruct_t setArray[DIM];
};

typedef struct setArrayStruct_t setArrayStruct_t;

struct dataInfo_t
{
  dataStruct_t data[DIM];
  uint32_t nb_of_data;
  uint32_t current;
};

typedef struct dataInfo_t dataInfo_t;
/* END TYPEDEFS */

/* STATIC VARIABLES */
static bool_t nullBool = FALSE;
static uint32_t globalVal;
static uint32_t* globalPtr = NULL;
infoStruct_t infoStructArray[DIM];
infoStruct_t defInfoStruct;
static format_t globalFormat = FMT_NULL;
static setStruct_t globalSet;
code_t globalCode;
uint32_t matrix[DIM][DIM];
static buffStruct_t* globalBuffStruct;
static setArrayStruct_t globalArraySet;
static pair_t globalPair;
static entry_t globalEntry;
static dataStruct_t globalDataStruct;
static dataInfo_t globalData;
static setStruct_t* globalSetPtr;
/*END STATIC VARIABLES */

/* This function initializes a vector: it sets the value of vect[i] as i + 1, with i=1...DIM */
void initVector(uint32_t* vectPtr)
{
    uint32_t i;
    if (vectPtr != NULL)
    {
        for (i = 0U; i < DIM; i++)
        {
            vectPtr[i] = INCR_VAL(i);
        }
    }
}

void segFaultVector(uint32_t* vectPtr)
{
    uint32_t i;
    vectPtr = 0;
    i = *vectPtr;
}
/* This function returns a vector structure.
    Its fields are filled with default values. */
vectStruct_t initVectStruct()
{
    vectStruct_t retStr;
    uint32_t i;
    retStr.id = 0U;
    retStr.init = FALSE;
    for (i = 0U; i < DIM; i++)
    {
        retStr.vector[i] = 0U;
    }
    return retStr;
}

bool_t checkVectStruct(vectStruct_t vectStr)
{
  bool_t isUpdated = FALSE;

  if (vectStr.init == TRUE)
    {
      isUpdated = TRUE;
      printf("VectStr has already been updated... id = %d\n", vectStr.id);
    }
  else
    {
      printf("VectStr not updated\n");
    }
  
  return isUpdated;
}

/* This function updates a vector structure
    with the values contained in a vectPtr.
    Furthermore, it updates the init flag
    (TRUE) and the id value.    */
void updateVectStruct(vectStruct_t* vectStr, uint32_t id, uint32_t* vectPtr, bool_t* updated)
{
    if (updated != NULL)
    {
        *updated = FALSE;
        /* Check if pointers are valid or not. */
        if ((vectStr != NULL) && (vectPtr != NULL))
        {
            vectStr->id = id;
            uint32_t i;
            for (i = 0U; i < DIM; i++)
            {
                vectStr->vector[i] = *(vectPtr+i);
            }
            vectStr->init = TRUE;
            *updated = TRUE;
        }
    }
    else
    {
        nullBool = TRUE;
    }
}

/* This function checks if the info structure array is valid (i.e.
    each info structure has a not null vector structure pointer)
    and return an array of valid/invalid values. */
valid_t* checkValidInfoStruct(infoStruct_t infoStrArray[NB_OF_STR])
{
    valid_t* validArray = (valid_t*)malloc(sizeof(valid_t) * NB_OF_STR);
    uint32_t i;
    for (i = 0U; i < NB_OF_STR; i++)
    {
        if (infoStrArray[i].vectorStruct != NULL)
        {
            validArray[i] = VALID;
        }
        else
        {
            validArray[i] = INVALID;
        }
    }
    return validArray;
}

/* This function returns the number of elements in
    an array of vector structures equal to the given
    element. */
uint32_t countOccVectStructArray(vectStruct_t vectStrArray[NB_OF_STR], 
                                                 uint32_t elem)
{
    uint32_t nb_of_occ = 0U;
    uint32_t i;
    uint32_t j;
    for (i = 0U; i < NB_OF_STR; i++)
    {
        for (j = 0U; j < DIM; j++)
        {
            if (vectStrArray[i].vector[j] == elem)
            {
                nb_of_occ++;
            }
        }
    }
    return nb_of_occ;
}

/* This function checks if the given vector structure
    has at least one field different from zero and, in
    case, returns 1; otherwise, it returns 0. */
static uint32_t testVectStruct(vectStruct_t* vectStruct)
{
    uint32_t retVal = 0U;
    uint32_t i;
    uint32_t count = 0U;
    /* Check if pointer is valid or not. */
    if (vectStruct != NULL)
    {
        for (i = 0U; i < DIM; i++)
        {
            if (vectStruct->vector[i] != 0U)
            {
                count++;
            }
        }
        if (count != 0U)
        {
            retVal = 1U;
        }
    }
    return retVal;
}

/* This function checks if the vector structure is NULL. */
static uint32_t testNullStruct(vectStruct_t* vectStruct)
{
    uint32_t retVal;
    if (vectStruct == NULL)
    {
        retVal = 1U;
    }
    else
    {
        retVal = 0U;
    }
    return retVal;
}

/* This function checks the given info structure by using
    the given test function and returns TRUE if the test
    passes, otherwise FALSE. */
bool_t testInfoStruct(infoStruct_t* infoStruct, uint32_t (* funct)(vectStruct_t* vectStruct))
{
    bool_t success = FALSE;
    uint32_t retVal = 0U;
    /* Check if pointer is valid or not. */
    if ((infoStruct != NULL) && (funct != NULL))
    {
        /* If key == 0, the check fails */
        if (infoStruct->key != 0U)
        {
            retVal = (*funct)(infoStruct->vectorStruct);
            if (retVal == 1U)
            {
                success = TRUE;
            }
        }
    }
    return success;
}

/* This function fills a vector with the negated values of the given number. */
void fillNegVect(uint32_t* vect, uint32_t num)
{
    uint32_t dummy;
    uint32_t negNum;
    uint32_t i;
    
    /* Check if vector is valid or not. */
    if (vect != NULL)
    {
        negNum = negVal(num, &dummy);
        
        for (i = 0U; i < DIM; i++)
        {
            vect[i] = negNum;
        }
        
    }
    
}

static void copyVector(uint32_t* outVect, uint32_t* inVect, uint32_t size)
{
    uint32_t i;
    uint32_t tmpSize = size;
    
    if ((inVect != NULL) && (outVect != NULL))
    {
        if (size > DIM)
        {
            tmpSize = DIM;
        }
        
        for (i = 0U; i < tmpSize; i++)
        {
            outVect[i] = inVect[i];
        }
    }
}

uint32_t returnPtrVal(uint32_t* numPtr, uint32_t val)
{
    if (numPtr != NULL)
    {
        *numPtr = val;
    }
    return val;
}

/* This function */
void buildNegValVector(uint32_t* vectPtr)
{
    uint32_t i;
    uint32_t dummy;
    uint32_t tmpVal;    
    uint32_t simpleVect[DIM] = {1U, 2U, 3U, 4U, 5U};

    if (vectPtr != NULL)
    {
        copyVector(vectPtr, &simpleVect[0U], DIM);
        for (i = 0U; i < DIM; i++)
        {
            tmpVal = negVal(*(vectPtr + i), &dummy);
            vectPtr[i] = tmpVal;
        }
    }
}

/* This function prints a message */
bool_t printMsg(char* message)
{
    bool_t retSuccess = FALSE;
    if (message != NULL)
    {
        printf(message);
        retSuccess = TRUE;
    }
    else
    {
        printf("ERROR: MESSAGE NOT VALID\n");
    }
    return retSuccess;
}

/* This functions calculates and returns the product of sums
    of the elements of three vectors (sum[i] = a[i] + b[i] + c[i]).
    The number of sums is equal to min(size,DIM). */    
uint32_t prodSum(uint32_t* v1, uint32_t v2[], uint32_t v3[DIM], uint32_t size)
{
    uint32_t retVal = 1U;
    /* Check if vectors are valid or not. */
    if ((v1 != NULL) && (v2 != NULL) && (v3 != NULL))
    {
        uint32_t realSize = size;
        if (size > DIM)
        {
            realSize = DIM;
        }
        uint32_t sumVect[realSize];
        uint32_t i;
        for (i = 0U; i < realSize; i++)
        {
            sumVect[i] = v1[i] + v2[i] + v3[i];
            retVal = retVal * sumVect[i];
        }
    }
    else
    {
        retVal = 0U;
    }
    return retVal;
}

bool_t isNegativeNumber(int32_t num)
{
  bool_t isNegative = FALSE;
  if (num < 0)
    {
      isNegative = TRUE;
    }
  return isNegative;
}

bool_t isWrittenToFile(int32_t fd, void* buff, uint32_t size)
{
  bool_t isWritten = FALSE;
  if (buff != NULL)
    {
      write(fd, buff, size);
      isWritten = TRUE;
    }
  return isWritten;
}

static void updateValue(uint32_t* value, uint32_t newVal)
{
  if (value != NULL)
    {
      *value = newVal;
    }
}

void updateVectorValues(uint32_t* vect, uint32_t val, uint32_t size)
{
  uint32_t i;

  if (vect != NULL)
    {
      for (i = 0U; i < size; i++)
	{
	  updateValue(&vect[i], i * val);
	}
    }
}

uint32_t returnGlobalPtrValue()
{
  uint32_t retVal;
  retVal = *globalPtr;
  return retVal;
}

static void printErrorMsg(char* msg, uint32_t size)
{
  write(stdout, msg, size);
  //printf("Printed Error Msg\n");
  //fflush(stdout);
}

void call_exit(uint32_t code)
{
  exit(code);
  //return;
}

void printError(void)
{
  printErrorMsg("ERROR",64);
}

uint32_t check_exit(uint32_t* ptr)
{
  struct timeval tv;
  int retVal = gettimeofday(&tv, NULL);
  char buffMsg[64U] = "[ ";
  char timeMsg[64U];
  snprintf(&timeMsg[0U], sizeof(timeMsg), "%d", tv.tv_sec);
  strcat(&buffMsg[0U], &timeMsg[0U]);
  strcat(&buffMsg[0U], " , ");
  snprintf(&timeMsg[0U], sizeof(timeMsg), "%d", tv.tv_sec);
  strcat(&buffMsg[0U], " ] ");
  strcat(&buffMsg[0U], "##ERROR: found a null ptr!!!##\n");
  if (ptr == NULL)
    {
      printErrorMsg(&buffMsg[0U], 64U);
      call_exit(0U);
    }
  return 1U;
}

uint32_t checkPtrIntegrity(uint32_t* ptr)
{
  uint32_t retVal = 1U;
  CHECK_EXIT(ptr);
  *ptr = 17U;
  return retVal;
}

uint32_t retNegFormat(format_t format)
{
  return ~(uint32_t)format;
}

format_t getGlobalFormat()
{
  return globalFormat;
}

void setGlobalCode(code_t code)
{
  globalCode = code;
}

code_t getGlobalCode()
{
  return globalCode;
}

void retDefInfoStruct(infoStruct_t** ptr)
{
  if (ptr!= NULL)
    {
      *ptr = &defInfoStruct;
    }
}

void retInfoStructEntry(infoStruct_t** ptr, uint32_t index)
{
  bool_t ok = TRUE;
  if (ptr == NULL)
    {
      ok = FALSE;
    }
  if (index >= DIM)
    {
      ok = FALSE;
    }
  if (ok == TRUE)
    {
      *ptr = &infoStructArray[index];
    }
}

bool_t setFirstInfoStructEntry(infoStruct_t** ptr)
{
  bool_t success = TRUE;
  if (ptr != NULL)
    {
      if (*ptr != NULL)
	{
	  infoStructArray[0U].key = (*ptr)->key;
	  infoStructArray[0U].vectorStruct->id = ((*ptr)->vectorStruct)->id;
	}
      else
	{
	  success = FALSE;
	}
    }
  else
    {
      success = FALSE;
    }
  return success;
}

void initInfoStructEntry(infoStruct_t* infoStrPtr, uint32_t index)
{
  vectStruct_t tmpVectStr;
  if (index < DIM)
    {
      infoStrPtr = &infoStructArray[index];
      infoStrPtr->key = 0U;
      tmpVectStr = initVectStruct();
      infoStrPtr->vectorStruct = &tmpVectStr;
    }
}

bool_t validInfoStructPtr(infoStruct_t* infoStrPtr)
{
  bool_t valid = FALSE;
  if (infoStrPtr != NULL)
    {
      if (infoStrPtr->vectorStruct != NULL)
	{
	  valid = TRUE;
	}
    }
  return valid;
}

bool_t isValidDataStruct(dataStruct_t* data)
{
  bool_t isValid = TRUE;

  uint32_t i;

  if (data != NULL)
    {
      if (data->format == FMT_NULL)
	{
	  isValid = FALSE;
	}
      if (((data->id == NULL) || (data->id[0] = '\0')) && isValid)
	{
	  isValid = FALSE;
	}
      if ((data->buffStruct.valid == INVALID) && isValid)
	{
	  isValid = FALSE;
	}
      for (i = 0U; i < BUFF_SIZE && isValid; i++)
	{
	  if (data->buffStruct.buff[i] == 0xDEADU)
	    {
	      isValid = FALSE;
	    }
	}
    }
  else
    {
      printf("Warning: passed a null ptr!\n");
      isValid = FALSE;
    }
  return isValid;
}

void getEmptyDataStruct(dataStruct_t* data)
{
  char tmpId = '\0';

  uint32_t i;

  if (data != NULL)
    {
      data->format = FMT_NULL;
      memcpy(data->id, &tmpId, sizeof(tmpId));
      data->buffStruct.valid = INVALID;
      
      for (i = 0U; i < BUFF_SIZE; i++)
	{
	  data->buffStruct.buff[i] = 0xDEADU;
	}
    }
}

void getEmptySetStruct(setStruct_t* set)
{
  if (set != NULL)
    {
      set->flag = FALSE;

      if (set->data != NULL)
	{
	  getEmptyDataStruct(set->data);
	}
    }
}

void retGlobalSet(setStruct_t* set)
{
  if (set != NULL)
    {
      memcpy(set, &globalSet, sizeof(globalSet));
    }
}

void setGlobalSet(setStruct_t set)
{
  memcpy(&globalSet, &set, sizeof(set));
}

void fillOnesArray(uint32_t array[3])
{
  uint32_t i;
  for (i = 0U; i < 3U; i++)
    {
      array[i] = 1U;
    }
}

void printCharArray(char array[DIM])
{
  printf("Array: %s\n",array);
}

void setMatrix(uint32_t** ptr)
{
  if (ptr != NULL)
    {
      if (*ptr != NULL)
	{
	  memcpy(&matrix[0U], *ptr, sizeof(matrix));
	}
    }
}

void getFirstEntryMatrix(uint32_t* ptr)
{
  if (ptr != NULL)
    {
      memcpy(ptr, &matrix[0U], sizeof(matrix[0U]));
    }
}

void setGlobalBuffStruct(buffStruct_t buffStruct)
{
  memcpy(globalBuffStruct, &buffStruct, sizeof(buffStruct));
}

void getGlobalBuffStuct(buffStruct_t* buffStruct)
{
  if (buffStruct != NULL)
    {
      memcpy(buffStruct, globalBuffStruct, sizeof(*buffStruct));
    }
}

void getGlobalArraySet(setArrayStruct_t* ptr)
{
  if (ptr != NULL)
    {
      memcpy(ptr, &globalArraySet, sizeof(globalArraySet));
    }
}

void copyArray(uint32_t outArray[3U], uint32_t inArray[3U])
{
  uint32_t i;
  for (i = 0U; i < 3U; i++)
    {
      outArray[i] = inArray[i];
    }
}

bool_t isZeroArray(array_t array)
{
  bool_t ret = TRUE;
  uint32_t i;
  for (i = 0U; i < 2U && !ret; i++)
    {
      if (array[i] != 0U)
	{
	  ret = FALSE;
	}
    }
  return ret;
}

void copyCharArray(char* out, char* in, uint32_t size)
{
  uint32_t i;
  for (i = 0U; i < size; i++)
    {
      out[i] = in[i];
    }
}

void copyCharArrayTwice(char* out, char* in_1, char* in_2, uint32_t size_1, uint32_t size_2)
{
  copyCharArray(out, in_1, size_1);
  copyCharArray(&out[size_1], in_2, size_2);
}

void getGlobalPair(pair_t* pair)
{
  if (pair != NULL)
    {
      memcpy(pair, &globalPair, sizeof(globalPair));
    }
}

void setGlobalPair(pair_t* pair)
{
  if (pair != NULL)
    {
      memcpy(&globalPair, pair, sizeof(globalPair));
    }
}

void copyPair(pair_t pair)
{
  memcpy(&globalPair, &pair, sizeof(pair));
}

void copyEntry(entry_t entry)
{
  memcpy(&globalEntry, &entry, sizeof(entry));
}

uint32_t countCharOcc(char msg[64], char c)
{
  uint32_t counter = 0U;
  uint32_t i;
  for (i = 0U; i < 64U; i++)
    {
      if (msg[i] == c)
	{
	  counter++;
	}
    }
  return counter;
}

bool_t setDefDataStruct(dataInfo_t dataInfo)
{
  bool_t success = TRUE;
  memcpy(&globalData, &dataInfo, sizeof(dataInfo));
  return success;
}

dataInfo_t getDefDataStruct()
{
  return globalData;
}

static void logg(char* message)
{
  printErrorMsg(message, 256);
}

void errorLog(char *msg) 
{
  logg(msg);
}

bool_t assertCheck(void* ptr, char* msg, char* file, int line, uint32_t checkVal)
{
  bool_t res = TRUE;
  if (ptr == NULL)
    {
      errorLog(msg);
      res = FALSE;
    }
  return res;
}

bool_t checkPtrs(void* ptr_1, void* ptr_2, void* ptr_3, char* file, int line, uint32_t checkVal)
{
  bool_t res;
  bool_t res_1;
  bool_t res_2;
  bool_t res_3;
  char msg[256] = "#abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ#@$%~()*&^ FATAL ERROR!!!!!! \n\t\r\n\nFOUND A NULL POINTER!!! SYSTEM ENTERS HALT STATE!!!! ########--/ERROR nfgkl1234567890`!@#$%^()*-_=+|{}{}[];;;nvmhfahu###\n";
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  /* Call again assertCheck */
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  /* Once more */
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res = (res_1 && res_2 && res_3);
  return res;
}

bool_t checkDataStructId(void)
{
  bool_t retVal = TRUE;
  if (globalDataStruct.id == NULL)
    {
      retVal = FALSE;
    }
  return retVal;
}

void getGlobalSetPtr(setStruct_t** ptr)
{
  if (ptr != NULL)
    {
      *ptr = globalSetPtr;
    }
}

/* Main program */
int main()
{
    uint32_t num = 10U;
    uint32_t retNum;
    uint32_t negNum = negVal(num, &retNum);
    uint32_t v1[] = {1, 2, 3};
    uint32_t v2[] = {4, 5, 6};
    uint32_t v3[DIM] = {7, 8, 9, 10, 11};
    uint32_t size = 3U;
    uint32_t retProdSum = prodSum(&v1[0U], v2, v3, size);
    printf("ProdSum value: %d\n",retProdSum);
    vectStruct_t v = initVectStruct();
    uint32_t i;
        
    printf("INITIALIZATION\n");
    for (i = 0U; i < DIM; i++)
    {
        printf("v.vector[%d]: %d\n",i,v.vector[i]);
    }
    printf("UPDATE\n");
    bool_t updated = FALSE;
    updateVectStruct(&v, 255U, &v3[0U],&updated);
    for (i = 0U; i < DIM; i++)
    {
        printf("v.vector[%d]: %d\n",i,v.vector[i]);
    }
    printf("updated: %d\n",updated);
    infoStruct_t infoStr;
    infoStr.key = 1U;
    infoStr.vectorStruct = &v;
    bool_t success = testInfoStruct(&infoStr, testVectStruct);
    printf("Info structure test result: %d\n",success);
    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "interface.h"
#include "types.h"
#include "utils.h"

/* MACRO DEFINITION */

#define DIM 5U
#define NB_OF_STR 3
#define BUFF_SIZE 64U

#define CHECK_EXIT(ptr) \
  do {		        \
    check_exit(ptr);    \
  } while (0)

/* END MACRO */

/* TYPEDEFS */
typedef uint32_t array_t[2];

array_t dummy_dummy;

typedef enum
  {
    FMT_NULL,
    FMT_DEC,
    FMT_CHAR
  } format_t;

typedef enum
  {
    CODE_0=255,
    CODE_1=1,
    CODE_2
  } code_t;

struct vectStruct_t
{
    uint32_t id;
    uint32_t vector[DIM];
    bool_t init;
};

typedef struct vectStruct_t vectStruct_t;

struct infoStruct_t
{
    uint32_t key;
    vectStruct_t* vectorStruct;
};

typedef struct infoStruct_t infoStruct_t;

typedef enum
{
    VALID,
    INVALID
} valid_t;

struct buffStruct_t
{
  valid_t valid;
  uint32_t buff[BUFF_SIZE];
};

typedef struct buffStruct_t buffStruct_t;

struct dataStruct_t
{
  format_t format;
  char* id;
  buffStruct_t buffStruct;
};

typedef struct dataStruct_t dataStruct_t;

struct setStruct_t
{
  bool_t flag;
  dataStruct_t* data;
};

typedef struct setStruct_t setStruct_t;

struct setArrayStruct_t
{
  uint32_t setArraySize;
  setStruct_t setArray[DIM];
};

typedef struct setArrayStruct_t setArrayStruct_t;

struct dataInfo_t
{
  dataStruct_t data[DIM];
  uint32_t nb_of_data;
  uint32_t current;
};

typedef struct dataInfo_t dataInfo_t;
/* END TYPEDEFS */

/* STATIC VARIABLES */
static bool_t nullBool = FALSE;
static uint32_t globalVal;
static uint32_t* globalPtr = NULL;
infoStruct_t infoStructArray[DIM];
infoStruct_t defInfoStruct;
static format_t globalFormat = FMT_NULL;
static setStruct_t globalSet;
code_t globalCode;
uint32_t matrix[DIM][DIM];
static buffStruct_t* globalBuffStruct;
static setArrayStruct_t globalArraySet;
static pair_t globalPair;
static entry_t globalEntry;
static dataStruct_t globalDataStruct;
static dataInfo_t globalData;
static setStruct_t* globalSetPtr;
/*END STATIC VARIABLES */

/* This function initializes a vector: it sets the value of vect[i] as i + 1, with i=1...DIM */
void initVector(uint32_t* vectPtr)
{
    uint32_t i;
    if (vectPtr != NULL)
    {
        for (i = 0U; i < DIM; i++)
        {
            vectPtr[i] = INCR_VAL(i);
        }
    }
}

void segFaultVector(uint32_t* vectPtr)
{
    uint32_t i;
    vectPtr = 0;
    i = *vectPtr;
}
/* This function returns a vector structure.
    Its fields are filled with default values. */
vectStruct_t initVectStruct()
{
    vectStruct_t retStr;
    uint32_t i;
    retStr.id = 0U;
    retStr.init = FALSE;
    for (i = 0U; i < DIM; i++)
    {
        retStr.vector[i] = 0U;
    }
    return retStr;
}

bool_t checkVectStruct(vectStruct_t vectStr)
{
  bool_t isUpdated = FALSE;

  if (vectStr.init == TRUE)
    {
      isUpdated = TRUE;
      printf("VectStr has already been updated... id = %d\n", vectStr.id);
    }
  else
    {
      printf("VectStr not updated\n");
    }
  
  return isUpdated;
}

/* This function updates a vector structure
    with the values contained in a vectPtr.
    Furthermore, it updates the init flag
    (TRUE) and the id value.    */
void updateVectStruct(vectStruct_t* vectStr, uint32_t id, uint32_t* vectPtr, bool_t* updated)
{
    if (updated != NULL)
    {
        *updated = FALSE;
        /* Check if pointers are valid or not. */
        if ((vectStr != NULL) && (vectPtr != NULL))
        {
            vectStr->id = id;
            uint32_t i;
            for (i = 0U; i < DIM; i++)
            {
                vectStr->vector[i] = *(vectPtr+i);
            }
            vectStr->init = TRUE;
            *updated = TRUE;
        }
    }
    else
    {
        nullBool = TRUE;
    }
}

/* This function checks if the info structure array is valid (i.e.
    each info structure has a not null vector structure pointer)
    and return an array of valid/invalid values. */
valid_t* checkValidInfoStruct(infoStruct_t infoStrArray[NB_OF_STR])
{
    valid_t* validArray = (valid_t*)malloc(sizeof(valid_t) * NB_OF_STR);
    uint32_t i;
    for (i = 0U; i < NB_OF_STR; i++)
    {
        if (infoStrArray[i].vectorStruct != NULL)
        {
            validArray[i] = VALID;
        }
        else
        {
            validArray[i] = INVALID;
        }
    }
    return validArray;
}

/* This function returns the number of elements in
    an array of vector structures equal to the given
    element. */
uint32_t countOccVectStructArray(vectStruct_t vectStrArray[NB_OF_STR], 
                                                 uint32_t elem)
{
    uint32_t nb_of_occ = 0U;
    uint32_t i;
    uint32_t j;
    for (i = 0U; i < NB_OF_STR; i++)
    {
        for (j = 0U; j < DIM; j++)
        {
            if (vectStrArray[i].vector[j] == elem)
            {
                nb_of_occ++;
            }
        }
    }
    return nb_of_occ;
}

/* This function checks if the given vector structure
    has at least one field different from zero and, in
    case, returns 1; otherwise, it returns 0. */
static uint32_t testVectStruct(vectStruct_t* vectStruct)
{
    uint32_t retVal = 0U;
    uint32_t i;
    uint32_t count = 0U;
    /* Check if pointer is valid or not. */
    if (vectStruct != NULL)
    {
        for (i = 0U; i < DIM; i++)
        {
            if (vectStruct->vector[i] != 0U)
            {
                count++;
            }
        }
        if (count != 0U)
        {
            retVal = 1U;
        }
    }
    return retVal;
}

/* This function checks if the vector structure is NULL. */
static uint32_t testNullStruct(vectStruct_t* vectStruct)
{
    uint32_t retVal;
    if (vectStruct == NULL)
    {
        retVal = 1U;
    }
    else
    {
        retVal = 0U;
    }
    return retVal;
}

/* This function checks the given info structure by using
    the given test function and returns TRUE if the test
    passes, otherwise FALSE. */
bool_t testInfoStruct(infoStruct_t* infoStruct, uint32_t (* funct)(vectStruct_t* vectStruct))
{
    bool_t success = FALSE;
    uint32_t retVal = 0U;
    /* Check if pointer is valid or not. */
    if ((infoStruct != NULL) && (funct != NULL))
    {
        /* If key == 0, the check fails */
        if (infoStruct->key != 0U)
        {
            retVal = (*funct)(infoStruct->vectorStruct);
            if (retVal == 1U)
            {
                success = TRUE;
            }
        }
    }
    return success;
}

/* This function fills a vector with the negated values of the given number. */
void fillNegVect(uint32_t* vect, uint32_t num)
{
    uint32_t dummy;
    uint32_t negNum;
    uint32_t i;
    
    /* Check if vector is valid or not. */
    if (vect != NULL)
    {
        negNum = negVal(num, &dummy);
        
        for (i = 0U; i < DIM; i++)
        {
            vect[i] = negNum;
        }
        
    }
    
}

static void copyVector(uint32_t* outVect, uint32_t* inVect, uint32_t size)
{
    uint32_t i;
    uint32_t tmpSize = size;
    
    if ((inVect != NULL) && (outVect != NULL))
    {
        if (size > DIM)
        {
            tmpSize = DIM;
        }
        
        for (i = 0U; i < tmpSize; i++)
        {
            outVect[i] = inVect[i];
        }
    }
}

uint32_t returnPtrVal(uint32_t* numPtr, uint32_t val)
{
    if (numPtr != NULL)
    {
        *numPtr = val;
    }
    return val;
}

/* This function */
void buildNegValVector(uint32_t* vectPtr)
{
    uint32_t i;
    uint32_t dummy;
    uint32_t tmpVal;    
    uint32_t simpleVect[DIM] = {1U, 2U, 3U, 4U, 5U};

    if (vectPtr != NULL)
    {
        copyVector(vectPtr, &simpleVect[0U], DIM);
        for (i = 0U; i < DIM; i++)
        {
            tmpVal = negVal(*(vectPtr + i), &dummy);
            vectPtr[i] = tmpVal;
        }
    }
}

/* This function prints a message */
bool_t printMsg(char* message)
{
    bool_t retSuccess = FALSE;
    if (message != NULL)
    {
        printf(message);
        retSuccess = TRUE;
    }
    else
    {
        printf("ERROR: MESSAGE NOT VALID\n");
    }
    return retSuccess;
}

/* This functions calculates and returns the product of sums
    of the elements of three vectors (sum[i] = a[i] + b[i] + c[i]).
    The number of sums is equal to min(size,DIM). */    
uint32_t prodSum(uint32_t* v1, uint32_t v2[], uint32_t v3[DIM], uint32_t size)
{
    uint32_t retVal = 1U;
    /* Check if vectors are valid or not. */
    if ((v1 != NULL) && (v2 != NULL) && (v3 != NULL))
    {
        uint32_t realSize = size;
        if (size > DIM)
        {
            realSize = DIM;
        }
        uint32_t sumVect[realSize];
        uint32_t i;
        for (i = 0U; i < realSize; i++)
        {
            sumVect[i] = v1[i] + v2[i] + v3[i];
            retVal = retVal * sumVect[i];
        }
    }
    else
    {
        retVal = 0U;
    }
    return retVal;
}

bool_t isNegativeNumber(int32_t num)
{
  bool_t isNegative = FALSE;
  if (num < 0)
    {
      isNegative = TRUE;
    }
  return isNegative;
}

bool_t isWrittenToFile(int32_t fd, void* buff, uint32_t size)
{
  bool_t isWritten = FALSE;
  if (buff != NULL)
    {
      write(fd, buff, size);
      isWritten = TRUE;
    }
  return isWritten;
}

static void updateValue(uint32_t* value, uint32_t newVal)
{
  if (value != NULL)
    {
      *value = newVal;
    }
}

void updateVectorValues(uint32_t* vect, uint32_t val, uint32_t size)
{
  uint32_t i;

  if (vect != NULL)
    {
      for (i = 0U; i < size; i++)
	{
	  updateValue(&vect[i], i * val);
	}
    }
}

uint32_t returnGlobalPtrValue()
{
  uint32_t retVal;
  retVal = *globalPtr;
  return retVal;
}

static void printErrorMsg(char* msg, uint32_t size)
{
  write(stdout, msg, size);
  //printf("Printed Error Msg\n");
  //fflush(stdout);
}

void call_exit(uint32_t code)
{
  exit(code);
  //return;
}

void printError(void)
{
  printErrorMsg("ERROR",64);
}

uint32_t check_exit(uint32_t* ptr)
{
  struct timeval tv;
  int retVal = gettimeofday(&tv, NULL);
  char buffMsg[64U] = "[ ";
  char timeMsg[64U];
  snprintf(&timeMsg[0U], sizeof(timeMsg), "%d", tv.tv_sec);
  strcat(&buffMsg[0U], &timeMsg[0U]);
  strcat(&buffMsg[0U], " , ");
  snprintf(&timeMsg[0U], sizeof(timeMsg), "%d", tv.tv_sec);
  strcat(&buffMsg[0U], " ] ");
  strcat(&buffMsg[0U], "##ERROR: found a null ptr!!!##\n");
  if (ptr == NULL)
    {
      printErrorMsg(&buffMsg[0U], 64U);
      call_exit(0U);
    }
  return 1U;
}

uint32_t checkPtrIntegrity(uint32_t* ptr)
{
  uint32_t retVal = 1U;
  CHECK_EXIT(ptr);
  *ptr = 17U;
  return retVal;
}

uint32_t retNegFormat(format_t format)
{
  return ~(uint32_t)format;
}

format_t getGlobalFormat()
{
  return globalFormat;
}

void setGlobalCode(code_t code)
{
  globalCode = code;
}

code_t getGlobalCode()
{
  return globalCode;
}

void retDefInfoStruct(infoStruct_t** ptr)
{
  if (ptr!= NULL)
    {
      *ptr = &defInfoStruct;
    }
}

void retInfoStructEntry(infoStruct_t** ptr, uint32_t index)
{
  bool_t ok = TRUE;
  if (ptr == NULL)
    {
      ok = FALSE;
    }
  if (index >= DIM)
    {
      ok = FALSE;
    }
  if (ok == TRUE)
    {
      *ptr = &infoStructArray[index];
    }
}

bool_t setFirstInfoStructEntry(infoStruct_t** ptr)
{
  bool_t success = TRUE;
  if (ptr != NULL)
    {
      if (*ptr != NULL)
	{
	  infoStructArray[0U].key = (*ptr)->key;
	  infoStructArray[0U].vectorStruct->id = ((*ptr)->vectorStruct)->id;
	}
      else
	{
	  success = FALSE;
	}
    }
  else
    {
      success = FALSE;
    }
  return success;
}

void initInfoStructEntry(infoStruct_t* infoStrPtr, uint32_t index)
{
  vectStruct_t tmpVectStr;
  if (index < DIM)
    {
      infoStrPtr = &infoStructArray[index];
      infoStrPtr->key = 0U;
      tmpVectStr = initVectStruct();
      infoStrPtr->vectorStruct = &tmpVectStr;
    }
}

bool_t validInfoStructPtr(infoStruct_t* infoStrPtr)
{
  bool_t valid = FALSE;
  if (infoStrPtr != NULL)
    {
      if (infoStrPtr->vectorStruct != NULL)
	{
	  valid = TRUE;
	}
    }
  return valid;
}

bool_t isValidDataStruct(dataStruct_t* data)
{
  bool_t isValid = TRUE;

  uint32_t i;

  if (data != NULL)
    {
      if (data->format == FMT_NULL)
	{
	  isValid = FALSE;
	}
      if (((data->id == NULL) || (data->id[0] = '\0')) && isValid)
	{
	  isValid = FALSE;
	}
      if ((data->buffStruct.valid == INVALID) && isValid)
	{
	  isValid = FALSE;
	}
      for (i = 0U; i < BUFF_SIZE && isValid; i++)
	{
	  if (data->buffStruct.buff[i] == 0xDEADU)
	    {
	      isValid = FALSE;
	    }
	}
    }
  else
    {
      printf("Warning: passed a null ptr!\n");
      isValid = FALSE;
    }
  return isValid;
}

void getEmptyDataStruct(dataStruct_t* data)
{
  char tmpId = '\0';

  uint32_t i;

  if (data != NULL)
    {
      data->format = FMT_NULL;
      memcpy(data->id, &tmpId, sizeof(tmpId));
      data->buffStruct.valid = INVALID;
      
      for (i = 0U; i < BUFF_SIZE; i++)
	{
	  data->buffStruct.buff[i] = 0xDEADU;
	}
    }
}

void getEmptySetStruct(setStruct_t* set)
{
  if (set != NULL)
    {
      set->flag = FALSE;

      if (set->data != NULL)
	{
	  getEmptyDataStruct(set->data);
	}
    }
}

void retGlobalSet(setStruct_t* set)
{
  if (set != NULL)
    {
      memcpy(set, &globalSet, sizeof(globalSet));
    }
}

void setGlobalSet(setStruct_t set)
{
  memcpy(&globalSet, &set, sizeof(set));
}

void fillOnesArray(uint32_t array[3])
{
  uint32_t i;
  for (i = 0U; i < 3U; i++)
    {
      array[i] = 1U;
    }
}

void printCharArray(char array[DIM])
{
  printf("Array: %s\n",array);
}

void setMatrix(uint32_t** ptr)
{
  if (ptr != NULL)
    {
      if (*ptr != NULL)
	{
	  memcpy(&matrix[0U], *ptr, sizeof(matrix));
	}
    }
}

void getFirstEntryMatrix(uint32_t* ptr)
{
  if (ptr != NULL)
    {
      memcpy(ptr, &matrix[0U], sizeof(matrix[0U]));
    }
}

void setGlobalBuffStruct(buffStruct_t buffStruct)
{
  memcpy(globalBuffStruct, &buffStruct, sizeof(buffStruct));
}

void getGlobalBuffStuct(buffStruct_t* buffStruct)
{
  if (buffStruct != NULL)
    {
      memcpy(buffStruct, globalBuffStruct, sizeof(*buffStruct));
    }
}

void getGlobalArraySet(setArrayStruct_t* ptr)
{
  if (ptr != NULL)
    {
      memcpy(ptr, &globalArraySet, sizeof(globalArraySet));
    }
}

void copyArray(uint32_t outArray[3U], uint32_t inArray[3U])
{
  uint32_t i;
  for (i = 0U; i < 3U; i++)
    {
      outArray[i] = inArray[i];
    }
}

bool_t isZeroArray(array_t array)
{
  bool_t ret = TRUE;
  uint32_t i;
  for (i = 0U; i < 2U && !ret; i++)
    {
      if (array[i] != 0U)
	{
	  ret = FALSE;
	}
    }
  return ret;
}

void copyCharArray(char* out, char* in, uint32_t size)
{
  uint32_t i;
  for (i = 0U; i < size; i++)
    {
      out[i] = in[i];
    }
}

void copyCharArrayTwice(char* out, char* in_1, char* in_2, uint32_t size_1, uint32_t size_2)
{
  copyCharArray(out, in_1, size_1);
  copyCharArray(&out[size_1], in_2, size_2);
}

void getGlobalPair(pair_t* pair)
{
  if (pair != NULL)
    {
      memcpy(pair, &globalPair, sizeof(globalPair));
    }
}

void setGlobalPair(pair_t* pair)
{
  if (pair != NULL)
    {
      memcpy(&globalPair, pair, sizeof(globalPair));
    }
}

void copyPair(pair_t pair)
{
  memcpy(&globalPair, &pair, sizeof(pair));
}

void copyEntry(entry_t entry)
{
  memcpy(&globalEntry, &entry, sizeof(entry));
}

uint32_t countCharOcc(char msg[64], char c)
{
  uint32_t counter = 0U;
  uint32_t i;
  for (i = 0U; i < 64U; i++)
    {
      if (msg[i] == c)
	{
	  counter++;
	}
    }
  return counter;
}

bool_t setDefDataStruct(dataInfo_t dataInfo)
{
  bool_t success = TRUE;
  memcpy(&globalData, &dataInfo, sizeof(dataInfo));
  return success;
}

dataInfo_t getDefDataStruct()
{
  return globalData;
}

static void logg(char* message)
{
  printErrorMsg(message, 256);
}

void errorLog(char *msg) 
{
  logg(msg);
}

bool_t assertCheck(void* ptr, char* msg, char* file, int line, uint32_t checkVal)
{
  bool_t res = TRUE;
  if (ptr == NULL)
    {
      errorLog(msg);
      res = FALSE;
    }
  return res;
}

bool_t checkPtrs(void* ptr_1, void* ptr_2, void* ptr_3, char* file, int line, uint32_t checkVal)
{
  bool_t res;
  bool_t res_1;
  bool_t res_2;
  bool_t res_3;
  char msg[256] = "#abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ#@$%~()*&^ FATAL ERROR!!!!!! \n\t\r\n\nFOUND A NULL POINTER!!! SYSTEM ENTERS HALT STATE!!!! ########--/ERROR nfgkl1234567890`!@#$%^()*-_=+|{}{}[];;;nvmhfahu###\n";
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  /* Call again assertCheck */
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  /* Once more */
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res = (res_1 && res_2 && res_3);
  return res;
}

bool_t checkDataStructId(void)
{
  bool_t retVal = TRUE;
  if (globalDataStruct.id == NULL)
    {
      retVal = FALSE;
    }
  return retVal;
}

void getGlobalSetPtr(setStruct_t** ptr)
{
  if (ptr != NULL)
    {
      *ptr = globalSetPtr;
    }
}

/* Main program */
int main()
{
    uint32_t num = 10U;
    uint32_t retNum;
    uint32_t negNum = negVal(num, &retNum);
    uint32_t v1[] = {1, 2, 3};
    uint32_t v2[] = {4, 5, 6};
    uint32_t v3[DIM] = {7, 8, 9, 10, 11};
    uint32_t size = 3U;
    uint32_t retProdSum = prodSum(&v1[0U], v2, v3, size);
    printf("ProdSum value: %d\n",retProdSum);
    vectStruct_t v = initVectStruct();
    uint32_t i;
        
    printf("INITIALIZATION\n");
    for (i = 0U; i < DIM; i++)
    {
        printf("v.vector[%d]: %d\n",i,v.vector[i]);
    }
    printf("UPDATE\n");
    bool_t updated = FALSE;
    updateVectStruct(&v, 255U, &v3[0U],&updated);
    for (i = 0U; i < DIM; i++)
    {
        printf("v.vector[%d]: %d\n",i,v.vector[i]);
    }
    printf("updated: %d\n",updated);
    infoStruct_t infoStr;
    infoStr.key = 1U;
    infoStr.vectorStruct = &v;
    bool_t success = testInfoStruct(&infoStr, testVectStruct);
    printf("Info structure test result: %d\n",success);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "interface.h"
#include "types.h"
#include "utils.h"

/* MACRO DEFINITION */

#define DIM 5U
#define NB_OF_STR 3
#define BUFF_SIZE 64U

#define CHECK_EXIT(ptr) \
  do {		        \
    check_exit(ptr);    \
  } while (0)

/* END MACRO */

/* TYPEDEFS */
typedef uint32_t array_t[2];

array_t dummy_dummy;

typedef enum
  {
    FMT_NULL,
    FMT_DEC,
    FMT_CHAR
  } format_t;

typedef enum
  {
    CODE_0=255,
    CODE_1=1,
    CODE_2
  } code_t;

struct vectStruct_t
{
    uint32_t id;
    uint32_t vector[DIM];
    bool_t init;
};

typedef struct vectStruct_t vectStruct_t;

struct infoStruct_t
{
    uint32_t key;
    vectStruct_t* vectorStruct;
};

typedef struct infoStruct_t infoStruct_t;

typedef enum
{
    VALID,
    INVALID
} valid_t;

struct buffStruct_t
{
  valid_t valid;
  uint32_t buff[BUFF_SIZE];
};

typedef struct buffStruct_t buffStruct_t;

struct dataStruct_t
{
  format_t format;
  char* id;
  buffStruct_t buffStruct;
};

typedef struct dataStruct_t dataStruct_t;

struct setStruct_t
{
  bool_t flag;
  dataStruct_t* data;
};

typedef struct setStruct_t setStruct_t;

struct setArrayStruct_t
{
  uint32_t setArraySize;
  setStruct_t setArray[DIM];
};

typedef struct setArrayStruct_t setArrayStruct_t;

struct dataInfo_t
{
  dataStruct_t data[DIM];
  uint32_t nb_of_data;
  uint32_t current;
};

typedef struct dataInfo_t dataInfo_t;
/* END TYPEDEFS */

/* STATIC VARIABLES */
static bool_t nullBool = FALSE;
static uint32_t globalVal;
static uint32_t* globalPtr = NULL;
infoStruct_t infoStructArray[DIM];
infoStruct_t defInfoStruct;
static format_t globalFormat = FMT_NULL;
static setStruct_t globalSet;
code_t globalCode;
uint32_t matrix[DIM][DIM];
static buffStruct_t* globalBuffStruct;
static setArrayStruct_t globalArraySet;
static pair_t globalPair;
static entry_t globalEntry;
static dataStruct_t globalDataStruct;
static dataInfo_t globalData;
static setStruct_t* globalSetPtr;
/*END STATIC VARIABLES */

/* This function initializes a vector: it sets the value of vect[i] as i + 1, with i=1...DIM */
void initVector(uint32_t* vectPtr)
{
    uint32_t i;
    if (vectPtr != NULL)
    {
        for (i = 0U; i < DIM; i++)
        {
            vectPtr[i] = INCR_VAL(i);
        }
    }
}

void segFaultVector(uint32_t* vectPtr)
{
    uint32_t i;
    vectPtr = 0;
    i = *vectPtr;
}
/* This function returns a vector structure.
    Its fields are filled with default values. */
vectStruct_t initVectStruct()
{
    vectStruct_t retStr;
    uint32_t i;
    retStr.id = 0U;
    retStr.init = FALSE;
    for (i = 0U; i < DIM; i++)
    {
        retStr.vector[i] = 0U;
    }
    return retStr;
}

bool_t checkVectStruct(vectStruct_t vectStr)
{
  bool_t isUpdated = FALSE;

  if (vectStr.init == TRUE)
    {
      isUpdated = TRUE;
      printf("VectStr has already been updated... id = %d\n", vectStr.id);
    }
  else
    {
      printf("VectStr not updated\n");
    }
  
  return isUpdated;
}

/* This function updates a vector structure
    with the values contained in a vectPtr.
    Furthermore, it updates the init flag
    (TRUE) and the id value.    */
void updateVectStruct(vectStruct_t* vectStr, uint32_t id, uint32_t* vectPtr, bool_t* updated)
{
    if (updated != NULL)
    {
        *updated = FALSE;
        /* Check if pointers are valid or not. */
        if ((vectStr != NULL) && (vectPtr != NULL))
        {
            vectStr->id = id;
            uint32_t i;
            for (i = 0U; i < DIM; i++)
            {
                vectStr->vector[i] = *(vectPtr+i);
            }
            vectStr->init = TRUE;
            *updated = TRUE;
        }
    }
    else
    {
        nullBool = TRUE;
    }
}

/* This function checks if the info structure array is valid (i.e.
    each info structure has a not null vector structure pointer)
    and return an array of valid/invalid values. */
valid_t* checkValidInfoStruct(infoStruct_t infoStrArray[NB_OF_STR])
{
    valid_t* validArray = (valid_t*)malloc(sizeof(valid_t) * NB_OF_STR);
    uint32_t i;
    for (i = 0U; i < NB_OF_STR; i++)
    {
        if (infoStrArray[i].vectorStruct != NULL)
        {
            validArray[i] = VALID;
        }
        else
        {
            validArray[i] = INVALID;
        }
    }
    return validArray;
}

/* This function returns the number of elements in
    an array of vector structures equal to the given
    element. */
uint32_t countOccVectStructArray(vectStruct_t vectStrArray[NB_OF_STR], 
                                                 uint32_t elem)
{
    uint32_t nb_of_occ = 0U;
    uint32_t i;
    uint32_t j;
    for (i = 0U; i < NB_OF_STR; i++)
    {
        for (j = 0U; j < DIM; j++)
        {
            if (vectStrArray[i].vector[j] == elem)
            {
                nb_of_occ++;
            }
        }
    }
    return nb_of_occ;
}

/* This function checks if the given vector structure
    has at least one field different from zero and, in
    case, returns 1; otherwise, it returns 0. */
static uint32_t testVectStruct(vectStruct_t* vectStruct)
{
    uint32_t retVal = 0U;
    uint32_t i;
    uint32_t count = 0U;
    /* Check if pointer is valid or not. */
    if (vectStruct != NULL)
    {
        for (i = 0U; i < DIM; i++)
        {
            if (vectStruct->vector[i] != 0U)
            {
                count++;
            }
        }
        if (count != 0U)
        {
            retVal = 1U;
        }
    }
    return retVal;
}

/* This function checks if the vector structure is NULL. */
static uint32_t testNullStruct(vectStruct_t* vectStruct)
{
    uint32_t retVal;
    if (vectStruct == NULL)
    {
        retVal = 1U;
    }
    else
    {
        retVal = 0U;
    }
    return retVal;
}

/* This function checks the given info structure by using
    the given test function and returns TRUE if the test
    passes, otherwise FALSE. */
bool_t testInfoStruct(infoStruct_t* infoStruct, uint32_t (* funct)(vectStruct_t* vectStruct))
{
    bool_t success = FALSE;
    uint32_t retVal = 0U;
    /* Check if pointer is valid or not. */
    if ((infoStruct != NULL) && (funct != NULL))
    {
        /* If key == 0, the check fails */
        if (infoStruct->key != 0U)
        {
            retVal = (*funct)(infoStruct->vectorStruct);
            if (retVal == 1U)
            {
                success = TRUE;
            }
        }
    }
    return success;
}

/* This function fills a vector with the negated values of the given number. */
void fillNegVect(uint32_t* vect, uint32_t num)
{
    uint32_t dummy;
    uint32_t negNum;
    uint32_t i;
    
    /* Check if vector is valid or not. */
    if (vect != NULL)
    {
        negNum = negVal(num, &dummy);
        
        for (i = 0U; i < DIM; i++)
        {
            vect[i] = negNum;
        }
        
    }
    
}

static void copyVector(uint32_t* outVect, uint32_t* inVect, uint32_t size)
{
    uint32_t i;
    uint32_t tmpSize = size;
    
    if ((inVect != NULL) && (outVect != NULL))
    {
        if (size > DIM)
        {
            tmpSize = DIM;
        }
        
        for (i = 0U; i < tmpSize; i++)
        {
            outVect[i] = inVect[i];
        }
    }
}

uint32_t returnPtrVal(uint32_t* numPtr, uint32_t val)
{
    if (numPtr != NULL)
    {
        *numPtr = val;
    }
    return val;
}

/* This function */
void buildNegValVector(uint32_t* vectPtr)
{
    uint32_t i;
    uint32_t dummy;
    uint32_t tmpVal;    
    uint32_t simpleVect[DIM] = {1U, 2U, 3U, 4U, 5U};

    if (vectPtr != NULL)
    {
        copyVector(vectPtr, &simpleVect[0U], DIM);
        for (i = 0U; i < DIM; i++)
        {
            tmpVal = negVal(*(vectPtr + i), &dummy);
            vectPtr[i] = tmpVal;
        }
    }
}

/* This function prints a message */
bool_t printMsg(char* message)
{
    bool_t retSuccess = FALSE;
    if (message != NULL)
    {
        printf(message);
        retSuccess = TRUE;
    }
    else
    {
        printf("ERROR: MESSAGE NOT VALID\n");
    }
    return retSuccess;
}

/* This functions calculates and returns the product of sums
    of the elements of three vectors (sum[i] = a[i] + b[i] + c[i]).
    The number of sums is equal to min(size,DIM). */    
uint32_t prodSum(uint32_t* v1, uint32_t v2[], uint32_t v3[DIM], uint32_t size)
{
    uint32_t retVal = 1U;
    /* Check if vectors are valid or not. */
    if ((v1 != NULL) && (v2 != NULL) && (v3 != NULL))
    {
        uint32_t realSize = size;
        if (size > DIM)
        {
            realSize = DIM;
        }
        uint32_t sumVect[realSize];
        uint32_t i;
        for (i = 0U; i < realSize; i++)
        {
            sumVect[i] = v1[i] + v2[i] + v3[i];
            retVal = retVal * sumVect[i];
        }
    }
    else
    {
        retVal = 0U;
    }
    return retVal;
}

bool_t isNegativeNumber(int32_t num)
{
  bool_t isNegative = FALSE;
  if (num < 0)
    {
      isNegative = TRUE;
    }
  return isNegative;
}

bool_t isWrittenToFile(int32_t fd, void* buff, uint32_t size)
{
  bool_t isWritten = FALSE;
  if (buff != NULL)
    {
      write(fd, buff, size);
      isWritten = TRUE;
    }
  return isWritten;
}

static void updateValue(uint32_t* value, uint32_t newVal)
{
  if (value != NULL)
    {
      *value = newVal;
    }
}

void updateVectorValues(uint32_t* vect, uint32_t val, uint32_t size)
{
  uint32_t i;

  if (vect != NULL)
    {
      for (i = 0U; i < size; i++)
	{
	  updateValue(&vect[i], i * val);
	}
    }
}

uint32_t returnGlobalPtrValue()
{
  uint32_t retVal;
  retVal = *globalPtr;
  return retVal;
}

static void printErrorMsg(char* msg, uint32_t size)
{
  write(stdout, msg, size);
  //printf("Printed Error Msg\n");
  //fflush(stdout);
}

void call_exit(uint32_t code)
{
  exit(code);
  //return;
}

void printError(void)
{
  printErrorMsg("ERROR",64);
}

uint32_t check_exit(uint32_t* ptr)
{
  struct timeval tv;
  int retVal = gettimeofday(&tv, NULL);
  char buffMsg[64U] = "[ ";
  char timeMsg[64U];
  snprintf(&timeMsg[0U], sizeof(timeMsg), "%d", tv.tv_sec);
  strcat(&buffMsg[0U], &timeMsg[0U]);
  strcat(&buffMsg[0U], " , ");
  snprintf(&timeMsg[0U], sizeof(timeMsg), "%d", tv.tv_sec);
  strcat(&buffMsg[0U], " ] ");
  strcat(&buffMsg[0U], "##ERROR: found a null ptr!!!##\n");
  if (ptr == NULL)
    {
      printErrorMsg(&buffMsg[0U], 64U);
      call_exit(0U);
    }
  return 1U;
}

uint32_t checkPtrIntegrity(uint32_t* ptr)
{
  uint32_t retVal = 1U;
  CHECK_EXIT(ptr);
  *ptr = 17U;
  return retVal;
}

uint32_t retNegFormat(format_t format)
{
  return ~(uint32_t)format;
}

format_t getGlobalFormat()
{
  return globalFormat;
}

void setGlobalCode(code_t code)
{
  globalCode = code;
}

code_t getGlobalCode()
{
  return globalCode;
}

void retDefInfoStruct(infoStruct_t** ptr)
{
  if (ptr!= NULL)
    {
      *ptr = &defInfoStruct;
    }
}

void retInfoStructEntry(infoStruct_t** ptr, uint32_t index)
{
  bool_t ok = TRUE;
  if (ptr == NULL)
    {
      ok = FALSE;
    }
  if (index >= DIM)
    {
      ok = FALSE;
    }
  if (ok == TRUE)
    {
      *ptr = &infoStructArray[index];
    }
}

bool_t setFirstInfoStructEntry(infoStruct_t** ptr)
{
  bool_t success = TRUE;
  if (ptr != NULL)
    {
      if (*ptr != NULL)
	{
	  infoStructArray[0U].key = (*ptr)->key;
	  infoStructArray[0U].vectorStruct->id = ((*ptr)->vectorStruct)->id;
	}
      else
	{
	  success = FALSE;
	}
    }
  else
    {
      success = FALSE;
    }
  return success;
}

void initInfoStructEntry(infoStruct_t* infoStrPtr, uint32_t index)
{
  vectStruct_t tmpVectStr;
  if (index < DIM)
    {
      infoStrPtr = &infoStructArray[index];
      infoStrPtr->key = 0U;
      tmpVectStr = initVectStruct();
      infoStrPtr->vectorStruct = &tmpVectStr;
    }
}

bool_t validInfoStructPtr(infoStruct_t* infoStrPtr)
{
  bool_t valid = FALSE;
  if (infoStrPtr != NULL)
    {
      if (infoStrPtr->vectorStruct != NULL)
	{
	  valid = TRUE;
	}
    }
  return valid;
}

bool_t isValidDataStruct(dataStruct_t* data)
{
  bool_t isValid = TRUE;

  uint32_t i;

  if (data != NULL)
    {
      if (data->format == FMT_NULL)
	{
	  isValid = FALSE;
	}
      if (((data->id == NULL) || (data->id[0] = '\0')) && isValid)
	{
	  isValid = FALSE;
	}
      if ((data->buffStruct.valid == INVALID) && isValid)
	{
	  isValid = FALSE;
	}
      for (i = 0U; i < BUFF_SIZE && isValid; i++)
	{
	  if (data->buffStruct.buff[i] == 0xDEADU)
	    {
	      isValid = FALSE;
	    }
	}
    }
  else
    {
      printf("Warning: passed a null ptr!\n");
      isValid = FALSE;
    }
  return isValid;
}

void getEmptyDataStruct(dataStruct_t* data)
{
  char tmpId = '\0';

  uint32_t i;

  if (data != NULL)
    {
      data->format = FMT_NULL;
      memcpy(data->id, &tmpId, sizeof(tmpId));
      data->buffStruct.valid = INVALID;
      
      for (i = 0U; i < BUFF_SIZE; i++)
	{
	  data->buffStruct.buff[i] = 0xDEADU;
	}
    }
}

void getEmptySetStruct(setStruct_t* set)
{
  if (set != NULL)
    {
      set->flag = FALSE;

      if (set->data != NULL)
	{
	  getEmptyDataStruct(set->data);
	}
    }
}

void retGlobalSet(setStruct_t* set)
{
  if (set != NULL)
    {
      memcpy(set, &globalSet, sizeof(globalSet));
    }
}

void setGlobalSet(setStruct_t set)
{
  memcpy(&globalSet, &set, sizeof(set));
}

void fillOnesArray(uint32_t array[3])
{
  uint32_t i;
  for (i = 0U; i < 3U; i++)
    {
      array[i] = 1U;
    }
}

void printCharArray(char array[DIM])
{
  printf("Array: %s\n",array);
}

void setMatrix(uint32_t** ptr)
{
  if (ptr != NULL)
    {
      if (*ptr != NULL)
	{
	  memcpy(&matrix[0U], *ptr, sizeof(matrix));
	}
    }
}

void getFirstEntryMatrix(uint32_t* ptr)
{
  if (ptr != NULL)
    {
      memcpy(ptr, &matrix[0U], sizeof(matrix[0U]));
    }
}

void setGlobalBuffStruct(buffStruct_t buffStruct)
{
  memcpy(globalBuffStruct, &buffStruct, sizeof(buffStruct));
}

void getGlobalBuffStuct(buffStruct_t* buffStruct)
{
  if (buffStruct != NULL)
    {
      memcpy(buffStruct, globalBuffStruct, sizeof(*buffStruct));
    }
}

void getGlobalArraySet(setArrayStruct_t* ptr)
{
  if (ptr != NULL)
    {
      memcpy(ptr, &globalArraySet, sizeof(globalArraySet));
    }
}

void copyArray(uint32_t outArray[3U], uint32_t inArray[3U])
{
  uint32_t i;
  for (i = 0U; i < 3U; i++)
    {
      outArray[i] = inArray[i];
    }
}

bool_t isZeroArray(array_t array)
{
  bool_t ret = TRUE;
  uint32_t i;
  for (i = 0U; i < 2U && !ret; i++)
    {
      if (array[i] != 0U)
	{
	  ret = FALSE;
	}
    }
  return ret;
}

void copyCharArray(char* out, char* in, uint32_t size)
{
  uint32_t i;
  for (i = 0U; i < size; i++)
    {
      out[i] = in[i];
    }
}

void copyCharArrayTwice(char* out, char* in_1, char* in_2, uint32_t size_1, uint32_t size_2)
{
  copyCharArray(out, in_1, size_1);
  copyCharArray(&out[size_1], in_2, size_2);
}

void getGlobalPair(pair_t* pair)
{
  if (pair != NULL)
    {
      memcpy(pair, &globalPair, sizeof(globalPair));
    }
}

void setGlobalPair(pair_t* pair)
{
  if (pair != NULL)
    {
      memcpy(&globalPair, pair, sizeof(globalPair));
    }
}

void copyPair(pair_t pair)
{
  memcpy(&globalPair, &pair, sizeof(pair));
}

void copyEntry(entry_t entry)
{
  memcpy(&globalEntry, &entry, sizeof(entry));
}

uint32_t countCharOcc(char msg[64], char c)
{
  uint32_t counter = 0U;
  uint32_t i;
  for (i = 0U; i < 64U; i++)
    {
      if (msg[i] == c)
	{
	  counter++;
	}
    }
  return counter;
}

bool_t setDefDataStruct(dataInfo_t dataInfo)
{
  bool_t success = TRUE;
  memcpy(&globalData, &dataInfo, sizeof(dataInfo));
  return success;
}

dataInfo_t getDefDataStruct()
{
  return globalData;
}

static void logg(char* message)
{
  printErrorMsg(message, 256);
}

void errorLog(char *msg) 
{
  logg(msg);
}

bool_t assertCheck(void* ptr, char* msg, char* file, int line, uint32_t checkVal)
{
  bool_t res = TRUE;
  if (ptr == NULL)
    {
      errorLog(msg);
      res = FALSE;
    }
  return res;
}

bool_t checkPtrs(void* ptr_1, void* ptr_2, void* ptr_3, char* file, int line, uint32_t checkVal)
{
  bool_t res;
  bool_t res_1;
  bool_t res_2;
  bool_t res_3;
  char msg[256] = "#abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ#@$%~()*&^ FATAL ERROR!!!!!! \n\t\r\n\nFOUND A NULL POINTER!!! SYSTEM ENTERS HALT STATE!!!! ########--/ERROR nfgkl1234567890`!@#$%^()*-_=+|{}{}[];;;nvmhfahu###\n";
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  /* Call again assertCheck */
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  /* Once more */
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res = (res_1 && res_2 && res_3);
  return res;
}

bool_t checkDataStructId(void)
{
  bool_t retVal = TRUE;
  if (globalDataStruct.id == NULL)
    {
      retVal = FALSE;
    }
  return retVal;
}

void getGlobalSetPtr(setStruct_t** ptr)
{
  if (ptr != NULL)
    {
      *ptr = globalSetPtr;
    }
}

/* Main program */
int main()
{
    uint32_t num = 10U;
    uint32_t retNum;
    uint32_t negNum = negVal(num, &retNum);
    uint32_t v1[] = {1, 2, 3};
    uint32_t v2[] = {4, 5, 6};
    uint32_t v3[DIM] = {7, 8, 9, 10, 11};
    uint32_t size = 3U;
    uint32_t retProdSum = prodSum(&v1[0U], v2, v3, size);
    printf("ProdSum value: %d\n",retProdSum);
    vectStruct_t v = initVectStruct();
    uint32_t i;
        
    printf("INITIALIZATION\n");
    for (i = 0U; i < DIM; i++)
    {
        printf("v.vector[%d]: %d\n",i,v.vector[i]);
    }
    printf("UPDATE\n");
    bool_t updated = FALSE;
    updateVectStruct(&v, 255U, &v3[0U],&updated);
    for (i = 0U; i < DIM; i++)
    {
        printf("v.vector[%d]: %d\n",i,v.vector[i]);
    }
    printf("updated: %d\n",updated);
    infoStruct_t infoStr;
    infoStr.key = 1U;
    infoStr.vectorStruct = &v;
    bool_t success = testInfoStruct(&infoStr, testVectStruct);
    printf("Info structure test result: %d\n",success);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "interface.h"
#include "types.h"
#include "utils.h"

/* MACRO DEFINITION */

#define DIM 5U
#define NB_OF_STR 3
#define BUFF_SIZE 64U

#define CHECK_EXIT(ptr) \
  do {		        \
    check_exit(ptr);    \
  } while (0)

/* END MACRO */

/* TYPEDEFS */
typedef uint32_t array_t[2];

array_t dummy_dummy;

typedef enum
  {
    FMT_NULL,
    FMT_DEC,
    FMT_CHAR
  } format_t;

typedef enum
  {
    CODE_0=255,
    CODE_1=1,
    CODE_2
  } code_t;

struct vectStruct_t
{
    uint32_t id;
    uint32_t vector[DIM];
    bool_t init;
};

typedef struct vectStruct_t vectStruct_t;

struct infoStruct_t
{
    uint32_t key;
    vectStruct_t* vectorStruct;
};

typedef struct infoStruct_t infoStruct_t;

typedef enum
{
    VALID,
    INVALID
} valid_t;

struct buffStruct_t
{
  valid_t valid;
  uint32_t buff[BUFF_SIZE];
};

typedef struct buffStruct_t buffStruct_t;

struct dataStruct_t
{
  format_t format;
  char* id;
  buffStruct_t buffStruct;
};

typedef struct dataStruct_t dataStruct_t;

struct setStruct_t
{
  bool_t flag;
  dataStruct_t* data;
};

typedef struct setStruct_t setStruct_t;

struct setArrayStruct_t
{
  uint32_t setArraySize;
  setStruct_t setArray[DIM];
};

typedef struct setArrayStruct_t setArrayStruct_t;

struct dataInfo_t
{
  dataStruct_t data[DIM];
  uint32_t nb_of_data;
  uint32_t current;
};

typedef struct dataInfo_t dataInfo_t;
/* END TYPEDEFS */

/* STATIC VARIABLES */
static bool_t nullBool = FALSE;
static uint32_t globalVal;
static uint32_t* globalPtr = NULL;
infoStruct_t infoStructArray[DIM];
infoStruct_t defInfoStruct;
static format_t globalFormat = FMT_NULL;
static setStruct_t globalSet;
code_t globalCode;
uint32_t matrix[DIM][DIM];
static buffStruct_t* globalBuffStruct;
static setArrayStruct_t globalArraySet;
static pair_t globalPair;
static entry_t globalEntry;
static dataStruct_t globalDataStruct;
static dataInfo_t globalData;
static setStruct_t* globalSetPtr;
/*END STATIC VARIABLES */

/* This function initializes a vector: it sets the value of vect[i] as i + 1, with i=1...DIM */
void initVector(uint32_t* vectPtr)
{
    uint32_t i;
    if (vectPtr != NULL)
    {
        for (i = 0U; i < DIM; i++)
        {
            vectPtr[i] = INCR_VAL(i);
        }
    }
}

void segFaultVector(uint32_t* vectPtr)
{
    uint32_t i;
    vectPtr = 0;
    i = *vectPtr;
}
/* This function returns a vector structure.
    Its fields are filled with default values. */
vectStruct_t initVectStruct()
{
    vectStruct_t retStr;
    uint32_t i;
    retStr.id = 0U;
    retStr.init = FALSE;
    for (i = 0U; i < DIM; i++)
    {
        retStr.vector[i] = 0U;
    }
    return retStr;
}

bool_t checkVectStruct(vectStruct_t vectStr)
{
  bool_t isUpdated = FALSE;

  if (vectStr.init == TRUE)
    {
      isUpdated = TRUE;
      printf("VectStr has already been updated... id = %d\n", vectStr.id);
    }
  else
    {
      printf("VectStr not updated\n");
    }
  
  return isUpdated;
}

/* This function updates a vector structure
    with the values contained in a vectPtr.
    Furthermore, it updates the init flag
    (TRUE) and the id value.    */
void updateVectStruct(vectStruct_t* vectStr, uint32_t id, uint32_t* vectPtr, bool_t* updated)
{
    if (updated != NULL)
    {
        *updated = FALSE;
        /* Check if pointers are valid or not. */
        if ((vectStr != NULL) && (vectPtr != NULL))
        {
            vectStr->id = id;
            uint32_t i;
            for (i = 0U; i < DIM; i++)
            {
                vectStr->vector[i] = *(vectPtr+i);
            }
            vectStr->init = TRUE;
            *updated = TRUE;
        }
    }
    else
    {
        nullBool = TRUE;
    }
}

/* This function checks if the info structure array is valid (i.e.
    each info structure has a not null vector structure pointer)
    and return an array of valid/invalid values. */
valid_t* checkValidInfoStruct(infoStruct_t infoStrArray[NB_OF_STR])
{
    valid_t* validArray = (valid_t*)malloc(sizeof(valid_t) * NB_OF_STR);
    uint32_t i;
    for (i = 0U; i < NB_OF_STR; i++)
    {
        if (infoStrArray[i].vectorStruct != NULL)
        {
            validArray[i] = VALID;
        }
        else
        {
            validArray[i] = INVALID;
        }
    }
    return validArray;
}

/* This function returns the number of elements in
    an array of vector structures equal to the given
    element. */
uint32_t countOccVectStructArray(vectStruct_t vectStrArray[NB_OF_STR], 
                                                 uint32_t elem)
{
    uint32_t nb_of_occ = 0U;
    uint32_t i;
    uint32_t j;
    for (i = 0U; i < NB_OF_STR; i++)
    {
        for (j = 0U; j < DIM; j++)
        {
            if (vectStrArray[i].vector[j] == elem)
            {
                nb_of_occ++;
            }
        }
    }
    return nb_of_occ;
}

/* This function checks if the given vector structure
    has at least one field different from zero and, in
    case, returns 1; otherwise, it returns 0. */
static uint32_t testVectStruct(vectStruct_t* vectStruct)
{
    uint32_t retVal = 0U;
    uint32_t i;
    uint32_t count = 0U;
    /* Check if pointer is valid or not. */
    if (vectStruct != NULL)
    {
        for (i = 0U; i < DIM; i++)
        {
            if (vectStruct->vector[i] != 0U)
            {
                count++;
            }
        }
        if (count != 0U)
        {
            retVal = 1U;
        }
    }
    return retVal;
}

/* This function checks if the vector structure is NULL. */
static uint32_t testNullStruct(vectStruct_t* vectStruct)
{
    uint32_t retVal;
    if (vectStruct == NULL)
    {
        retVal = 1U;
    }
    else
    {
        retVal = 0U;
    }
    return retVal;
}

/* This function checks the given info structure by using
    the given test function and returns TRUE if the test
    passes, otherwise FALSE. */
bool_t testInfoStruct(infoStruct_t* infoStruct, uint32_t (* funct)(vectStruct_t* vectStruct))
{
    bool_t success = FALSE;
    uint32_t retVal = 0U;
    /* Check if pointer is valid or not. */
    if ((infoStruct != NULL) && (funct != NULL))
    {
        /* If key == 0, the check fails */
        if (infoStruct->key != 0U)
        {
            retVal = (*funct)(infoStruct->vectorStruct);
            if (retVal == 1U)
            {
                success = TRUE;
            }
        }
    }
    return success;
}

/* This function fills a vector with the negated values of the given number. */
void fillNegVect(uint32_t* vect, uint32_t num)
{
    uint32_t dummy;
    uint32_t negNum;
    uint32_t i;
    
    /* Check if vector is valid or not. */
    if (vect != NULL)
    {
        negNum = negVal(num, &dummy);
        
        for (i = 0U; i < DIM; i++)
        {
            vect[i] = negNum;
        }
        
    }
    
}

static void copyVector(uint32_t* outVect, uint32_t* inVect, uint32_t size)
{
    uint32_t i;
    uint32_t tmpSize = size;
    
    if ((inVect != NULL) && (outVect != NULL))
    {
        if (size > DIM)
        {
            tmpSize = DIM;
        }
        
        for (i = 0U; i < tmpSize; i++)
        {
            outVect[i] = inVect[i];
        }
    }
}

uint32_t returnPtrVal(uint32_t* numPtr, uint32_t val)
{
    if (numPtr != NULL)
    {
        *numPtr = val;
    }
    return val;
}

/* This function */
void buildNegValVector(uint32_t* vectPtr)
{
    uint32_t i;
    uint32_t dummy;
    uint32_t tmpVal;    
    uint32_t simpleVect[DIM] = {1U, 2U, 3U, 4U, 5U};

    if (vectPtr != NULL)
    {
        copyVector(vectPtr, &simpleVect[0U], DIM);
        for (i = 0U; i < DIM; i++)
        {
            tmpVal = negVal(*(vectPtr + i), &dummy);
            vectPtr[i] = tmpVal;
        }
    }
}

/* This function prints a message */
bool_t printMsg(char* message)
{
    bool_t retSuccess = FALSE;
    if (message != NULL)
    {
        printf(message);
        retSuccess = TRUE;
    }
    else
    {
        printf("ERROR: MESSAGE NOT VALID\n");
    }
    return retSuccess;
}

/* This functions calculates and returns the product of sums
    of the elements of three vectors (sum[i] = a[i] + b[i] + c[i]).
    The number of sums is equal to min(size,DIM). */    
uint32_t prodSum(uint32_t* v1, uint32_t v2[], uint32_t v3[DIM], uint32_t size)
{
    uint32_t retVal = 1U;
    /* Check if vectors are valid or not. */
    if ((v1 != NULL) && (v2 != NULL) && (v3 != NULL))
    {
        uint32_t realSize = size;
        if (size > DIM)
        {
            realSize = DIM;
        }
        uint32_t sumVect[realSize];
        uint32_t i;
        for (i = 0U; i < realSize; i++)
        {
            sumVect[i] = v1[i] + v2[i] + v3[i];
            retVal = retVal * sumVect[i];
        }
    }
    else
    {
        retVal = 0U;
    }
    return retVal;
}

bool_t isNegativeNumber(int32_t num)
{
  bool_t isNegative = FALSE;
  if (num < 0)
    {
      isNegative = TRUE;
    }
  return isNegative;
}

bool_t isWrittenToFile(int32_t fd, void* buff, uint32_t size)
{
  bool_t isWritten = FALSE;
  if (buff != NULL)
    {
      write(fd, buff, size);
      isWritten = TRUE;
    }
  return isWritten;
}

static void updateValue(uint32_t* value, uint32_t newVal)
{
  if (value != NULL)
    {
      *value = newVal;
    }
}

void updateVectorValues(uint32_t* vect, uint32_t val, uint32_t size)
{
  uint32_t i;

  if (vect != NULL)
    {
      for (i = 0U; i < size; i++)
	{
	  updateValue(&vect[i], i * val);
	}
    }
}

uint32_t returnGlobalPtrValue()
{
  uint32_t retVal;
  retVal = *globalPtr;
  return retVal;
}

static void printErrorMsg(char* msg, uint32_t size)
{
  write(stdout, msg, size);
  //printf("Printed Error Msg\n");
  //fflush(stdout);
}

void call_exit(uint32_t code)
{
  exit(code);
  //return;
}

void printError(void)
{
  printErrorMsg("ERROR",64);
}

uint32_t check_exit(uint32_t* ptr)
{
  struct timeval tv;
  int retVal = gettimeofday(&tv, NULL);
  char buffMsg[64U] = "[ ";
  char timeMsg[64U];
  snprintf(&timeMsg[0U], sizeof(timeMsg), "%d", tv.tv_sec);
  strcat(&buffMsg[0U], &timeMsg[0U]);
  strcat(&buffMsg[0U], " , ");
  snprintf(&timeMsg[0U], sizeof(timeMsg), "%d", tv.tv_sec);
  strcat(&buffMsg[0U], " ] ");
  strcat(&buffMsg[0U], "##ERROR: found a null ptr!!!##\n");
  if (ptr == NULL)
    {
      printErrorMsg(&buffMsg[0U], 64U);
      call_exit(0U);
    }
  return 1U;
}

uint32_t checkPtrIntegrity(uint32_t* ptr)
{
  uint32_t retVal = 1U;
  CHECK_EXIT(ptr);
  *ptr = 17U;
  return retVal;
}

uint32_t retNegFormat(format_t format)
{
  return ~(uint32_t)format;
}

format_t getGlobalFormat()
{
  return globalFormat;
}

void setGlobalCode(code_t code)
{
  globalCode = code;
}

code_t getGlobalCode()
{
  return globalCode;
}

void retDefInfoStruct(infoStruct_t** ptr)
{
  if (ptr!= NULL)
    {
      *ptr = &defInfoStruct;
    }
}

void retInfoStructEntry(infoStruct_t** ptr, uint32_t index)
{
  bool_t ok = TRUE;
  if (ptr == NULL)
    {
      ok = FALSE;
    }
  if (index >= DIM)
    {
      ok = FALSE;
    }
  if (ok == TRUE)
    {
      *ptr = &infoStructArray[index];
    }
}

bool_t setFirstInfoStructEntry(infoStruct_t** ptr)
{
  bool_t success = TRUE;
  if (ptr != NULL)
    {
      if (*ptr != NULL)
	{
	  infoStructArray[0U].key = (*ptr)->key;
	  infoStructArray[0U].vectorStruct->id = ((*ptr)->vectorStruct)->id;
	}
      else
	{
	  success = FALSE;
	}
    }
  else
    {
      success = FALSE;
    }
  return success;
}

void initInfoStructEntry(infoStruct_t* infoStrPtr, uint32_t index)
{
  vectStruct_t tmpVectStr;
  if (index < DIM)
    {
      infoStrPtr = &infoStructArray[index];
      infoStrPtr->key = 0U;
      tmpVectStr = initVectStruct();
      infoStrPtr->vectorStruct = &tmpVectStr;
    }
}

bool_t validInfoStructPtr(infoStruct_t* infoStrPtr)
{
  bool_t valid = FALSE;
  if (infoStrPtr != NULL)
    {
      if (infoStrPtr->vectorStruct != NULL)
	{
	  valid = TRUE;
	}
    }
  return valid;
}

bool_t isValidDataStruct(dataStruct_t* data)
{
  bool_t isValid = TRUE;

  uint32_t i;

  if (data != NULL)
    {
      if (data->format == FMT_NULL)
	{
	  isValid = FALSE;
	}
      if (((data->id == NULL) || (data->id[0] = '\0')) && isValid)
	{
	  isValid = FALSE;
	}
      if ((data->buffStruct.valid == INVALID) && isValid)
	{
	  isValid = FALSE;
	}
      for (i = 0U; i < BUFF_SIZE && isValid; i++)
	{
	  if (data->buffStruct.buff[i] == 0xDEADU)
	    {
	      isValid = FALSE;
	    }
	}
    }
  else
    {
      printf("Warning: passed a null ptr!\n");
      isValid = FALSE;
    }
  return isValid;
}

void getEmptyDataStruct(dataStruct_t* data)
{
  char tmpId = '\0';

  uint32_t i;

  if (data != NULL)
    {
      data->format = FMT_NULL;
      memcpy(data->id, &tmpId, sizeof(tmpId));
      data->buffStruct.valid = INVALID;
      
      for (i = 0U; i < BUFF_SIZE; i++)
	{
	  data->buffStruct.buff[i] = 0xDEADU;
	}
    }
}

void getEmptySetStruct(setStruct_t* set)
{
  if (set != NULL)
    {
      set->flag = FALSE;

      if (set->data != NULL)
	{
	  getEmptyDataStruct(set->data);
	}
    }
}

void retGlobalSet(setStruct_t* set)
{
  if (set != NULL)
    {
      memcpy(set, &globalSet, sizeof(globalSet));
    }
}

void setGlobalSet(setStruct_t set)
{
  memcpy(&globalSet, &set, sizeof(set));
}

void fillOnesArray(uint32_t array[3])
{
  uint32_t i;
  for (i = 0U; i < 3U; i++)
    {
      array[i] = 1U;
    }
}

void printCharArray(char array[DIM])
{
  printf("Array: %s\n",array);
}

void setMatrix(uint32_t** ptr)
{
  if (ptr != NULL)
    {
      if (*ptr != NULL)
	{
	  memcpy(&matrix[0U], *ptr, sizeof(matrix));
	}
    }
}

void getFirstEntryMatrix(uint32_t* ptr)
{
  if (ptr != NULL)
    {
      memcpy(ptr, &matrix[0U], sizeof(matrix[0U]));
    }
}

void setGlobalBuffStruct(buffStruct_t buffStruct)
{
  memcpy(globalBuffStruct, &buffStruct, sizeof(buffStruct));
}

void getGlobalBuffStuct(buffStruct_t* buffStruct)
{
  if (buffStruct != NULL)
    {
      memcpy(buffStruct, globalBuffStruct, sizeof(*buffStruct));
    }
}

void getGlobalArraySet(setArrayStruct_t* ptr)
{
  if (ptr != NULL)
    {
      memcpy(ptr, &globalArraySet, sizeof(globalArraySet));
    }
}

void copyArray(uint32_t outArray[3U], uint32_t inArray[3U])
{
  uint32_t i;
  for (i = 0U; i < 3U; i++)
    {
      outArray[i] = inArray[i];
    }
}

bool_t isZeroArray(array_t array)
{
  bool_t ret = TRUE;
  uint32_t i;
  for (i = 0U; i < 2U && !ret; i++)
    {
      if (array[i] != 0U)
	{
	  ret = FALSE;
	}
    }
  return ret;
}

void copyCharArray(char* out, char* in, uint32_t size)
{
  uint32_t i;
  for (i = 0U; i < size; i++)
    {
      out[i] = in[i];
    }
}

void copyCharArrayTwice(char* out, char* in_1, char* in_2, uint32_t size_1, uint32_t size_2)
{
  copyCharArray(out, in_1, size_1);
  copyCharArray(&out[size_1], in_2, size_2);
}

void getGlobalPair(pair_t* pair)
{
  if (pair != NULL)
    {
      memcpy(pair, &globalPair, sizeof(globalPair));
    }
}

void setGlobalPair(pair_t* pair)
{
  if (pair != NULL)
    {
      memcpy(&globalPair, pair, sizeof(globalPair));
    }
}

void copyPair(pair_t pair)
{
  memcpy(&globalPair, &pair, sizeof(pair));
}

void copyEntry(entry_t entry)
{
  memcpy(&globalEntry, &entry, sizeof(entry));
}

uint32_t countCharOcc(char msg[64], char c)
{
  uint32_t counter = 0U;
  uint32_t i;
  for (i = 0U; i < 64U; i++)
    {
      if (msg[i] == c)
	{
	  counter++;
	}
    }
  return counter;
}

bool_t setDefDataStruct(dataInfo_t dataInfo)
{
  bool_t success = TRUE;
  memcpy(&globalData, &dataInfo, sizeof(dataInfo));
  return success;
}

dataInfo_t getDefDataStruct()
{
  return globalData;
}

static void logg(char* message)
{
  printErrorMsg(message, 256);
}

void errorLog(char *msg) 
{
  logg(msg);
}

bool_t assertCheck(void* ptr, char* msg, char* file, int line, uint32_t checkVal)
{
  bool_t res = TRUE;
  if (ptr == NULL)
    {
      errorLog(msg);
      res = FALSE;
    }
  return res;
}

bool_t checkPtrs(void* ptr_1, void* ptr_2, void* ptr_3, char* file, int line, uint32_t checkVal)
{
  bool_t res;
  bool_t res_1;
  bool_t res_2;
  bool_t res_3;
  char msg[256] = "#abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ#@$%~()*&^ FATAL ERROR!!!!!! \n\t\r\n\nFOUND A NULL POINTER!!! SYSTEM ENTERS HALT STATE!!!! ########--/ERROR nfgkl1234567890`!@#$%^()*-_=+|{}{}[];;;nvmhfahu###\n";
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  /* Call again assertCheck */
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  /* Once more */
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res = (res_1 && res_2 && res_3);
  return res;
}

bool_t checkDataStructId(void)
{
  bool_t retVal = TRUE;
  if (globalDataStruct.id == NULL)
    {
      retVal = FALSE;
    }
  return retVal;
}

void getGlobalSetPtr(setStruct_t** ptr)
{
  if (ptr != NULL)
    {
      *ptr = globalSetPtr;
    }
}

/* Main program */
int main()
{
    uint32_t num = 10U;
    uint32_t retNum;
    uint32_t negNum = negVal(num, &retNum);
    uint32_t v1[] = {1, 2, 3};
    uint32_t v2[] = {4, 5, 6};
    uint32_t v3[DIM] = {7, 8, 9, 10, 11};
    uint32_t size = 3U;
    uint32_t retProdSum = prodSum(&v1[0U], v2, v3, size);
    printf("ProdSum value: %d\n",retProdSum);
    vectStruct_t v = initVectStruct();
    uint32_t i;
        
    printf("INITIALIZATION\n");
    for (i = 0U; i < DIM; i++)
    {
        printf("v.vector[%d]: %d\n",i,v.vector[i]);
    }
    printf("UPDATE\n");
    bool_t updated = FALSE;
    updateVectStruct(&v, 255U, &v3[0U],&updated);
    for (i = 0U; i < DIM; i++)
    {
        printf("v.vector[%d]: %d\n",i,v.vector[i]);
    }
    printf("updated: %d\n",updated);
    infoStruct_t infoStr;
    infoStr.key = 1U;
    infoStr.vectorStruct = &v;
    bool_t success = testInfoStruct(&infoStr, testVectStruct);
    printf("Info structure test result: %d\n",success);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "interface.h"
#include "types.h"
#include "utils.h"

/* MACRO DEFINITION */

#define DIM 5U
#define NB_OF_STR 3
#define BUFF_SIZE 64U

#define CHECK_EXIT(ptr) \
  do {		        \
    check_exit(ptr);    \
  } while (0)

/* END MACRO */

/* TYPEDEFS */
typedef uint32_t array_t[2];

array_t dummy_dummy;

typedef enum
  {
    FMT_NULL,
    FMT_DEC,
    FMT_CHAR
  } format_t;

typedef enum
  {
    CODE_0=255,
    CODE_1=1,
    CODE_2
  } code_t;

struct vectStruct_t
{
    uint32_t id;
    uint32_t vector[DIM];
    bool_t init;
};

typedef struct vectStruct_t vectStruct_t;

struct infoStruct_t
{
    uint32_t key;
    vectStruct_t* vectorStruct;
};

typedef struct infoStruct_t infoStruct_t;

typedef enum
{
    VALID,
    INVALID
} valid_t;

struct buffStruct_t
{
  valid_t valid;
  uint32_t buff[BUFF_SIZE];
};

typedef struct buffStruct_t buffStruct_t;

struct dataStruct_t
{
  format_t format;
  char* id;
  buffStruct_t buffStruct;
};

typedef struct dataStruct_t dataStruct_t;

struct setStruct_t
{
  bool_t flag;
  dataStruct_t* data;
};

typedef struct setStruct_t setStruct_t;

struct setArrayStruct_t
{
  uint32_t setArraySize;
  setStruct_t setArray[DIM];
};

typedef struct setArrayStruct_t setArrayStruct_t;

struct dataInfo_t
{
  dataStruct_t data[DIM];
  uint32_t nb_of_data;
  uint32_t current;
};

typedef struct dataInfo_t dataInfo_t;
/* END TYPEDEFS */

/* STATIC VARIABLES */
static bool_t nullBool = FALSE;
static uint32_t globalVal;
static uint32_t* globalPtr = NULL;
infoStruct_t infoStructArray[DIM];
infoStruct_t defInfoStruct;
static format_t globalFormat = FMT_NULL;
static setStruct_t globalSet;
code_t globalCode;
uint32_t matrix[DIM][DIM];
static buffStruct_t* globalBuffStruct;
static setArrayStruct_t globalArraySet;
static pair_t globalPair;
static entry_t globalEntry;
static dataStruct_t globalDataStruct;
static dataInfo_t globalData;
static setStruct_t* globalSetPtr;
/*END STATIC VARIABLES */

/* This function initializes a vector: it sets the value of vect[i] as i + 1, with i=1...DIM */
void initVector(uint32_t* vectPtr)
{
    uint32_t i;
    if (vectPtr != NULL)
    {
        for (i = 0U; i < DIM; i++)
        {
            vectPtr[i] = INCR_VAL(i);
        }
    }
}

void segFaultVector(uint32_t* vectPtr)
{
    uint32_t i;
    vectPtr = 0;
    i = *vectPtr;
}
/* This function returns a vector structure.
    Its fields are filled with default values. */
vectStruct_t initVectStruct()
{
    vectStruct_t retStr;
    uint32_t i;
    retStr.id = 0U;
    retStr.init = FALSE;
    for (i = 0U; i < DIM; i++)
    {
        retStr.vector[i] = 0U;
    }
    return retStr;
}

bool_t checkVectStruct(vectStruct_t vectStr)
{
  bool_t isUpdated = FALSE;

  if (vectStr.init == TRUE)
    {
      isUpdated = TRUE;
      printf("VectStr has already been updated... id = %d\n", vectStr.id);
    }
  else
    {
      printf("VectStr not updated\n");
    }
  
  return isUpdated;
}

/* This function updates a vector structure
    with the values contained in a vectPtr.
    Furthermore, it updates the init flag
    (TRUE) and the id value.    */
void updateVectStruct(vectStruct_t* vectStr, uint32_t id, uint32_t* vectPtr, bool_t* updated)
{
    if (updated != NULL)
    {
        *updated = FALSE;
        /* Check if pointers are valid or not. */
        if ((vectStr != NULL) && (vectPtr != NULL))
        {
            vectStr->id = id;
            uint32_t i;
            for (i = 0U; i < DIM; i++)
            {
                vectStr->vector[i] = *(vectPtr+i);
            }
            vectStr->init = TRUE;
            *updated = TRUE;
        }
    }
    else
    {
        nullBool = TRUE;
    }
}

/* This function checks if the info structure array is valid (i.e.
    each info structure has a not null vector structure pointer)
    and return an array of valid/invalid values. */
valid_t* checkValidInfoStruct(infoStruct_t infoStrArray[NB_OF_STR])
{
    valid_t* validArray = (valid_t*)malloc(sizeof(valid_t) * NB_OF_STR);
    uint32_t i;
    for (i = 0U; i < NB_OF_STR; i++)
    {
        if (infoStrArray[i].vectorStruct != NULL)
        {
            validArray[i] = VALID;
        }
        else
        {
            validArray[i] = INVALID;
        }
    }
    return validArray;
}

/* This function returns the number of elements in
    an array of vector structures equal to the given
    element. */
uint32_t countOccVectStructArray(vectStruct_t vectStrArray[NB_OF_STR], 
                                                 uint32_t elem)
{
    uint32_t nb_of_occ = 0U;
    uint32_t i;
    uint32_t j;
    for (i = 0U; i < NB_OF_STR; i++)
    {
        for (j = 0U; j < DIM; j++)
        {
            if (vectStrArray[i].vector[j] == elem)
            {
                nb_of_occ++;
            }
        }
    }
    return nb_of_occ;
}

/* This function checks if the given vector structure
    has at least one field different from zero and, in
    case, returns 1; otherwise, it returns 0. */
static uint32_t testVectStruct(vectStruct_t* vectStruct)
{
    uint32_t retVal = 0U;
    uint32_t i;
    uint32_t count = 0U;
    /* Check if pointer is valid or not. */
    if (vectStruct != NULL)
    {
        for (i = 0U; i < DIM; i++)
        {
            if (vectStruct->vector[i] != 0U)
            {
                count++;
            }
        }
        if (count != 0U)
        {
            retVal = 1U;
        }
    }
    return retVal;
}

/* This function checks if the vector structure is NULL. */
static uint32_t testNullStruct(vectStruct_t* vectStruct)
{
    uint32_t retVal;
    if (vectStruct == NULL)
    {
        retVal = 1U;
    }
    else
    {
        retVal = 0U;
    }
    return retVal;
}

/* This function checks the given info structure by using
    the given test function and returns TRUE if the test
    passes, otherwise FALSE. */
bool_t testInfoStruct(infoStruct_t* infoStruct, uint32_t (* funct)(vectStruct_t* vectStruct))
{
    bool_t success = FALSE;
    uint32_t retVal = 0U;
    /* Check if pointer is valid or not. */
    if ((infoStruct != NULL) && (funct != NULL))
    {
        /* If key == 0, the check fails */
        if (infoStruct->key != 0U)
        {
            retVal = (*funct)(infoStruct->vectorStruct);
            if (retVal == 1U)
            {
                success = TRUE;
            }
        }
    }
    return success;
}

/* This function fills a vector with the negated values of the given number. */
void fillNegVect(uint32_t* vect, uint32_t num)
{
    uint32_t dummy;
    uint32_t negNum;
    uint32_t i;
    
    /* Check if vector is valid or not. */
    if (vect != NULL)
    {
        negNum = negVal(num, &dummy);
        
        for (i = 0U; i < DIM; i++)
        {
            vect[i] = negNum;
        }
        
    }
    
}

static void copyVector(uint32_t* outVect, uint32_t* inVect, uint32_t size)
{
    uint32_t i;
    uint32_t tmpSize = size;
    
    if ((inVect != NULL) && (outVect != NULL))
    {
        if (size > DIM)
        {
            tmpSize = DIM;
        }
        
        for (i = 0U; i < tmpSize; i++)
        {
            outVect[i] = inVect[i];
        }
    }
}

uint32_t returnPtrVal(uint32_t* numPtr, uint32_t val)
{
    if (numPtr != NULL)
    {
        *numPtr = val;
    }
    return val;
}

/* This function */
void buildNegValVector(uint32_t* vectPtr)
{
    uint32_t i;
    uint32_t dummy;
    uint32_t tmpVal;    
    uint32_t simpleVect[DIM] = {1U, 2U, 3U, 4U, 5U};

    if (vectPtr != NULL)
    {
        copyVector(vectPtr, &simpleVect[0U], DIM);
        for (i = 0U; i < DIM; i++)
        {
            tmpVal = negVal(*(vectPtr + i), &dummy);
            vectPtr[i] = tmpVal;
        }
    }
}

/* This function prints a message */
bool_t printMsg(char* message)
{
    bool_t retSuccess = FALSE;
    if (message != NULL)
    {
        printf(message);
        retSuccess = TRUE;
    }
    else
    {
        printf("ERROR: MESSAGE NOT VALID\n");
    }
    return retSuccess;
}

/* This functions calculates and returns the product of sums
    of the elements of three vectors (sum[i] = a[i] + b[i] + c[i]).
    The number of sums is equal to min(size,DIM). */    
uint32_t prodSum(uint32_t* v1, uint32_t v2[], uint32_t v3[DIM], uint32_t size)
{
    uint32_t retVal = 1U;
    /* Check if vectors are valid or not. */
    if ((v1 != NULL) && (v2 != NULL) && (v3 != NULL))
    {
        uint32_t realSize = size;
        if (size > DIM)
        {
            realSize = DIM;
        }
        uint32_t sumVect[realSize];
        uint32_t i;
        for (i = 0U; i < realSize; i++)
        {
            sumVect[i] = v1[i] + v2[i] + v3[i];
            retVal = retVal * sumVect[i];
        }
    }
    else
    {
        retVal = 0U;
    }
    return retVal;
}

bool_t isNegativeNumber(int32_t num)
{
  bool_t isNegative = FALSE;
  if (num < 0)
    {
      isNegative = TRUE;
    }
  return isNegative;
}

bool_t isWrittenToFile(int32_t fd, void* buff, uint32_t size)
{
  bool_t isWritten = FALSE;
  if (buff != NULL)
    {
      write(fd, buff, size);
      isWritten = TRUE;
    }
  return isWritten;
}

static void updateValue(uint32_t* value, uint32_t newVal)
{
  if (value != NULL)
    {
      *value = newVal;
    }
}

void updateVectorValues(uint32_t* vect, uint32_t val, uint32_t size)
{
  uint32_t i;

  if (vect != NULL)
    {
      for (i = 0U; i < size; i++)
	{
	  updateValue(&vect[i], i * val);
	}
    }
}

uint32_t returnGlobalPtrValue()
{
  uint32_t retVal;
  retVal = *globalPtr;
  return retVal;
}

static void printErrorMsg(char* msg, uint32_t size)
{
  write(stdout, msg, size);
  //printf("Printed Error Msg\n");
  //fflush(stdout);
}

void call_exit(uint32_t code)
{
  exit(code);
  //return;
}

void printError(void)
{
  printErrorMsg("ERROR",64);
}

uint32_t check_exit(uint32_t* ptr)
{
  struct timeval tv;
  int retVal = gettimeofday(&tv, NULL);
  char buffMsg[64U] = "[ ";
  char timeMsg[64U];
  snprintf(&timeMsg[0U], sizeof(timeMsg), "%d", tv.tv_sec);
  strcat(&buffMsg[0U], &timeMsg[0U]);
  strcat(&buffMsg[0U], " , ");
  snprintf(&timeMsg[0U], sizeof(timeMsg), "%d", tv.tv_sec);
  strcat(&buffMsg[0U], " ] ");
  strcat(&buffMsg[0U], "##ERROR: found a null ptr!!!##\n");
  if (ptr == NULL)
    {
      printErrorMsg(&buffMsg[0U], 64U);
      call_exit(0U);
    }
  return 1U;
}

uint32_t checkPtrIntegrity(uint32_t* ptr)
{
  uint32_t retVal = 1U;
  CHECK_EXIT(ptr);
  *ptr = 17U;
  return retVal;
}

uint32_t retNegFormat(format_t format)
{
  return ~(uint32_t)format;
}

format_t getGlobalFormat()
{
  return globalFormat;
}

void setGlobalCode(code_t code)
{
  globalCode = code;
}

code_t getGlobalCode()
{
  return globalCode;
}

void retDefInfoStruct(infoStruct_t** ptr)
{
  if (ptr!= NULL)
    {
      *ptr = &defInfoStruct;
    }
}

void retInfoStructEntry(infoStruct_t** ptr, uint32_t index)
{
  bool_t ok = TRUE;
  if (ptr == NULL)
    {
      ok = FALSE;
    }
  if (index >= DIM)
    {
      ok = FALSE;
    }
  if (ok == TRUE)
    {
      *ptr = &infoStructArray[index];
    }
}

bool_t setFirstInfoStructEntry(infoStruct_t** ptr)
{
  bool_t success = TRUE;
  if (ptr != NULL)
    {
      if (*ptr != NULL)
	{
	  infoStructArray[0U].key = (*ptr)->key;
	  infoStructArray[0U].vectorStruct->id = ((*ptr)->vectorStruct)->id;
	}
      else
	{
	  success = FALSE;
	}
    }
  else
    {
      success = FALSE;
    }
  return success;
}

void initInfoStructEntry(infoStruct_t* infoStrPtr, uint32_t index)
{
  vectStruct_t tmpVectStr;
  if (index < DIM)
    {
      infoStrPtr = &infoStructArray[index];
      infoStrPtr->key = 0U;
      tmpVectStr = initVectStruct();
      infoStrPtr->vectorStruct = &tmpVectStr;
    }
}

bool_t validInfoStructPtr(infoStruct_t* infoStrPtr)
{
  bool_t valid = FALSE;
  if (infoStrPtr != NULL)
    {
      if (infoStrPtr->vectorStruct != NULL)
	{
	  valid = TRUE;
	}
    }
  return valid;
}

bool_t isValidDataStruct(dataStruct_t* data)
{
  bool_t isValid = TRUE;

  uint32_t i;

  if (data != NULL)
    {
      if (data->format == FMT_NULL)
	{
	  isValid = FALSE;
	}
      if (((data->id == NULL) || (data->id[0] = '\0')) && isValid)
	{
	  isValid = FALSE;
	}
      if ((data->buffStruct.valid == INVALID) && isValid)
	{
	  isValid = FALSE;
	}
      for (i = 0U; i < BUFF_SIZE && isValid; i++)
	{
	  if (data->buffStruct.buff[i] == 0xDEADU)
	    {
	      isValid = FALSE;
	    }
	}
    }
  else
    {
      printf("Warning: passed a null ptr!\n");
      isValid = FALSE;
    }
  return isValid;
}

void getEmptyDataStruct(dataStruct_t* data)
{
  char tmpId = '\0';

  uint32_t i;

  if (data != NULL)
    {
      data->format = FMT_NULL;
      memcpy(data->id, &tmpId, sizeof(tmpId));
      data->buffStruct.valid = INVALID;
      
      for (i = 0U; i < BUFF_SIZE; i++)
	{
	  data->buffStruct.buff[i] = 0xDEADU;
	}
    }
}

void getEmptySetStruct(setStruct_t* set)
{
  if (set != NULL)
    {
      set->flag = FALSE;

      if (set->data != NULL)
	{
	  getEmptyDataStruct(set->data);
	}
    }
}

void retGlobalSet(setStruct_t* set)
{
  if (set != NULL)
    {
      memcpy(set, &globalSet, sizeof(globalSet));
    }
}

void setGlobalSet(setStruct_t set)
{
  memcpy(&globalSet, &set, sizeof(set));
}

void fillOnesArray(uint32_t array[3])
{
  uint32_t i;
  for (i = 0U; i < 3U; i++)
    {
      array[i] = 1U;
    }
}

void printCharArray(char array[DIM])
{
  printf("Array: %s\n",array);
}

void setMatrix(uint32_t** ptr)
{
  if (ptr != NULL)
    {
      if (*ptr != NULL)
	{
	  memcpy(&matrix[0U], *ptr, sizeof(matrix));
	}
    }
}

void getFirstEntryMatrix(uint32_t* ptr)
{
  if (ptr != NULL)
    {
      memcpy(ptr, &matrix[0U], sizeof(matrix[0U]));
    }
}

void setGlobalBuffStruct(buffStruct_t buffStruct)
{
  memcpy(globalBuffStruct, &buffStruct, sizeof(buffStruct));
}

void getGlobalBuffStuct(buffStruct_t* buffStruct)
{
  if (buffStruct != NULL)
    {
      memcpy(buffStruct, globalBuffStruct, sizeof(*buffStruct));
    }
}

void getGlobalArraySet(setArrayStruct_t* ptr)
{
  if (ptr != NULL)
    {
      memcpy(ptr, &globalArraySet, sizeof(globalArraySet));
    }
}

void copyArray(uint32_t outArray[3U], uint32_t inArray[3U])
{
  uint32_t i;
  for (i = 0U; i < 3U; i++)
    {
      outArray[i] = inArray[i];
    }
}

bool_t isZeroArray(array_t array)
{
  bool_t ret = TRUE;
  uint32_t i;
  for (i = 0U; i < 2U && !ret; i++)
    {
      if (array[i] != 0U)
	{
	  ret = FALSE;
	}
    }
  return ret;
}

void copyCharArray(char* out, char* in, uint32_t size)
{
  uint32_t i;
  for (i = 0U; i < size; i++)
    {
      out[i] = in[i];
    }
}

void copyCharArrayTwice(char* out, char* in_1, char* in_2, uint32_t size_1, uint32_t size_2)
{
  copyCharArray(out, in_1, size_1);
  copyCharArray(&out[size_1], in_2, size_2);
}

void getGlobalPair(pair_t* pair)
{
  if (pair != NULL)
    {
      memcpy(pair, &globalPair, sizeof(globalPair));
    }
}

void setGlobalPair(pair_t* pair)
{
  if (pair != NULL)
    {
      memcpy(&globalPair, pair, sizeof(globalPair));
    }
}

void copyPair(pair_t pair)
{
  memcpy(&globalPair, &pair, sizeof(pair));
}

void copyEntry(entry_t entry)
{
  memcpy(&globalEntry, &entry, sizeof(entry));
}

uint32_t countCharOcc(char msg[64], char c)
{
  uint32_t counter = 0U;
  uint32_t i;
  for (i = 0U; i < 64U; i++)
    {
      if (msg[i] == c)
	{
	  counter++;
	}
    }
  return counter;
}

bool_t setDefDataStruct(dataInfo_t dataInfo)
{
  bool_t success = TRUE;
  memcpy(&globalData, &dataInfo, sizeof(dataInfo));
  return success;
}

dataInfo_t getDefDataStruct()
{
  return globalData;
}

static void logg(char* message)
{
  printErrorMsg(message, 256);
}

void errorLog(char *msg) 
{
  logg(msg);
}

bool_t assertCheck(void* ptr, char* msg, char* file, int line, uint32_t checkVal)
{
  bool_t res = TRUE;
  if (ptr == NULL)
    {
      errorLog(msg);
      res = FALSE;
    }
  return res;
}

bool_t checkPtrs(void* ptr_1, void* ptr_2, void* ptr_3, char* file, int line, uint32_t checkVal)
{
  bool_t res;
  bool_t res_1;
  bool_t res_2;
  bool_t res_3;
  char msg[256] = "#abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ#@$%~()*&^ FATAL ERROR!!!!!! \n\t\r\n\nFOUND A NULL POINTER!!! SYSTEM ENTERS HALT STATE!!!! ########--/ERROR nfgkl1234567890`!@#$%^()*-_=+|{}{}[];;;nvmhfahu###\n";
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  /* Call again assertCheck */
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  /* Once more */
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res = (res_1 && res_2 && res_3);
  return res;
}

bool_t checkDataStructId(void)
{
  bool_t retVal = TRUE;
  if (globalDataStruct.id == NULL)
    {
      retVal = FALSE;
    }
  return retVal;
}

void getGlobalSetPtr(setStruct_t** ptr)
{
  if (ptr != NULL)
    {
      *ptr = globalSetPtr;
    }
}

/* Main program */
int main()
{
    uint32_t num = 10U;
    uint32_t retNum;
    uint32_t negNum = negVal(num, &retNum);
    uint32_t v1[] = {1, 2, 3};
    uint32_t v2[] = {4, 5, 6};
    uint32_t v3[DIM] = {7, 8, 9, 10, 11};
    uint32_t size = 3U;
    uint32_t retProdSum = prodSum(&v1[0U], v2, v3, size);
    printf("ProdSum value: %d\n",retProdSum);
    vectStruct_t v = initVectStruct();
    uint32_t i;
        
    printf("INITIALIZATION\n");
    for (i = 0U; i < DIM; i++)
    {
        printf("v.vector[%d]: %d\n",i,v.vector[i]);
    }
    printf("UPDATE\n");
    bool_t updated = FALSE;
    updateVectStruct(&v, 255U, &v3[0U],&updated);
    for (i = 0U; i < DIM; i++)
    {
        printf("v.vector[%d]: %d\n",i,v.vector[i]);
    }
    printf("updated: %d\n",updated);
    infoStruct_t infoStr;
    infoStr.key = 1U;
    infoStr.vectorStruct = &v;
    bool_t success = testInfoStruct(&infoStr, testVectStruct);
    printf("Info structure test result: %d\n",success);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "interface.h"
#include "types.h"
#include "utils.h"

/* MACRO DEFINITION */

#define DIM 5U
#define NB_OF_STR 3
#define BUFF_SIZE 64U

#define CHECK_EXIT(ptr) \
  do {		        \
    check_exit(ptr);    \
  } while (0)

/* END MACRO */

/* TYPEDEFS */
typedef uint32_t array_t[2];

array_t dummy_dummy;

typedef enum
  {
    FMT_NULL,
    FMT_DEC,
    FMT_CHAR
  } format_t;

typedef enum
  {
    CODE_0=255,
    CODE_1=1,
    CODE_2
  } code_t;

struct vectStruct_t
{
    uint32_t id;
    uint32_t vector[DIM];
    bool_t init;
};

typedef struct vectStruct_t vectStruct_t;

struct infoStruct_t
{
    uint32_t key;
    vectStruct_t* vectorStruct;
};

typedef struct infoStruct_t infoStruct_t;

typedef enum
{
    VALID,
    INVALID
} valid_t;

struct buffStruct_t
{
  valid_t valid;
  uint32_t buff[BUFF_SIZE];
};

typedef struct buffStruct_t buffStruct_t;

struct dataStruct_t
{
  format_t format;
  char* id;
  buffStruct_t buffStruct;
};

typedef struct dataStruct_t dataStruct_t;

struct setStruct_t
{
  bool_t flag;
  dataStruct_t* data;
};

typedef struct setStruct_t setStruct_t;

struct setArrayStruct_t
{
  uint32_t setArraySize;
  setStruct_t setArray[DIM];
};

typedef struct setArrayStruct_t setArrayStruct_t;

struct dataInfo_t
{
  dataStruct_t data[DIM];
  uint32_t nb_of_data;
  uint32_t current;
};

typedef struct dataInfo_t dataInfo_t;
/* END TYPEDEFS */

/* STATIC VARIABLES */
static bool_t nullBool = FALSE;
static uint32_t globalVal;
static uint32_t* globalPtr = NULL;
infoStruct_t infoStructArray[DIM];
infoStruct_t defInfoStruct;
static format_t globalFormat = FMT_NULL;
static setStruct_t globalSet;
code_t globalCode;
uint32_t matrix[DIM][DIM];
static buffStruct_t* globalBuffStruct;
static setArrayStruct_t globalArraySet;
static pair_t globalPair;
static entry_t globalEntry;
static dataStruct_t globalDataStruct;
static dataInfo_t globalData;
static setStruct_t* globalSetPtr;
/*END STATIC VARIABLES */

/* This function initializes a vector: it sets the value of vect[i] as i + 1, with i=1...DIM */
void initVector(uint32_t* vectPtr)
{
    uint32_t i;
    if (vectPtr != NULL)
    {
        for (i = 0U; i < DIM; i++)
        {
            vectPtr[i] = INCR_VAL(i);
        }
    }
}

void segFaultVector(uint32_t* vectPtr)
{
    uint32_t i;
    vectPtr = 0;
    i = *vectPtr;
}
/* This function returns a vector structure.
    Its fields are filled with default values. */
vectStruct_t initVectStruct()
{
    vectStruct_t retStr;
    uint32_t i;
    retStr.id = 0U;
    retStr.init = FALSE;
    for (i = 0U; i < DIM; i++)
    {
        retStr.vector[i] = 0U;
    }
    return retStr;
}

bool_t checkVectStruct(vectStruct_t vectStr)
{
  bool_t isUpdated = FALSE;

  if (vectStr.init == TRUE)
    {
      isUpdated = TRUE;
      printf("VectStr has already been updated... id = %d\n", vectStr.id);
    }
  else
    {
      printf("VectStr not updated\n");
    }
  
  return isUpdated;
}

/* This function updates a vector structure
    with the values contained in a vectPtr.
    Furthermore, it updates the init flag
    (TRUE) and the id value.    */
void updateVectStruct(vectStruct_t* vectStr, uint32_t id, uint32_t* vectPtr, bool_t* updated)
{
    if (updated != NULL)
    {
        *updated = FALSE;
        /* Check if pointers are valid or not. */
        if ((vectStr != NULL) && (vectPtr != NULL))
        {
            vectStr->id = id;
            uint32_t i;
            for (i = 0U; i < DIM; i++)
            {
                vectStr->vector[i] = *(vectPtr+i);
            }
            vectStr->init = TRUE;
            *updated = TRUE;
        }
    }
    else
    {
        nullBool = TRUE;
    }
}

/* This function checks if the info structure array is valid (i.e.
    each info structure has a not null vector structure pointer)
    and return an array of valid/invalid values. */
valid_t* checkValidInfoStruct(infoStruct_t infoStrArray[NB_OF_STR])
{
    valid_t* validArray = (valid_t*)malloc(sizeof(valid_t) * NB_OF_STR);
    uint32_t i;
    for (i = 0U; i < NB_OF_STR; i++)
    {
        if (infoStrArray[i].vectorStruct != NULL)
        {
            validArray[i] = VALID;
        }
        else
        {
            validArray[i] = INVALID;
        }
    }
    return validArray;
}

/* This function returns the number of elements in
    an array of vector structures equal to the given
    element. */
uint32_t countOccVectStructArray(vectStruct_t vectStrArray[NB_OF_STR], 
                                                 uint32_t elem)
{
    uint32_t nb_of_occ = 0U;
    uint32_t i;
    uint32_t j;
    for (i = 0U; i < NB_OF_STR; i++)
    {
        for (j = 0U; j < DIM; j++)
        {
            if (vectStrArray[i].vector[j] == elem)
            {
                nb_of_occ++;
            }
        }
    }
    return nb_of_occ;
}

/* This function checks if the given vector structure
    has at least one field different from zero and, in
    case, returns 1; otherwise, it returns 0. */
static uint32_t testVectStruct(vectStruct_t* vectStruct)
{
    uint32_t retVal = 0U;
    uint32_t i;
    uint32_t count = 0U;
    /* Check if pointer is valid or not. */
    if (vectStruct != NULL)
    {
        for (i = 0U; i < DIM; i++)
        {
            if (vectStruct->vector[i] != 0U)
            {
                count++;
            }
        }
        if (count != 0U)
        {
            retVal = 1U;
        }
    }
    return retVal;
}

/* This function checks if the vector structure is NULL. */
static uint32_t testNullStruct(vectStruct_t* vectStruct)
{
    uint32_t retVal;
    if (vectStruct == NULL)
    {
        retVal = 1U;
    }
    else
    {
        retVal = 0U;
    }
    return retVal;
}

/* This function checks the given info structure by using
    the given test function and returns TRUE if the test
    passes, otherwise FALSE. */
bool_t testInfoStruct(infoStruct_t* infoStruct, uint32_t (* funct)(vectStruct_t* vectStruct))
{
    bool_t success = FALSE;
    uint32_t retVal = 0U;
    /* Check if pointer is valid or not. */
    if ((infoStruct != NULL) && (funct != NULL))
    {
        /* If key == 0, the check fails */
        if (infoStruct->key != 0U)
        {
            retVal = (*funct)(infoStruct->vectorStruct);
            if (retVal == 1U)
            {
                success = TRUE;
            }
        }
    }
    return success;
}

/* This function fills a vector with the negated values of the given number. */
void fillNegVect(uint32_t* vect, uint32_t num)
{
    uint32_t dummy;
    uint32_t negNum;
    uint32_t i;
    
    /* Check if vector is valid or not. */
    if (vect != NULL)
    {
        negNum = negVal(num, &dummy);
        
        for (i = 0U; i < DIM; i++)
        {
            vect[i] = negNum;
        }
        
    }
    
}

static void copyVector(uint32_t* outVect, uint32_t* inVect, uint32_t size)
{
    uint32_t i;
    uint32_t tmpSize = size;
    
    if ((inVect != NULL) && (outVect != NULL))
    {
        if (size > DIM)
        {
            tmpSize = DIM;
        }
        
        for (i = 0U; i < tmpSize; i++)
        {
            outVect[i] = inVect[i];
        }
    }
}

uint32_t returnPtrVal(uint32_t* numPtr, uint32_t val)
{
    if (numPtr != NULL)
    {
        *numPtr = val;
    }
    return val;
}

/* This function */
void buildNegValVector(uint32_t* vectPtr)
{
    uint32_t i;
    uint32_t dummy;
    uint32_t tmpVal;    
    uint32_t simpleVect[DIM] = {1U, 2U, 3U, 4U, 5U};

    if (vectPtr != NULL)
    {
        copyVector(vectPtr, &simpleVect[0U], DIM);
        for (i = 0U; i < DIM; i++)
        {
            tmpVal = negVal(*(vectPtr + i), &dummy);
            vectPtr[i] = tmpVal;
        }
    }
}

/* This function prints a message */
bool_t printMsg(char* message)
{
    bool_t retSuccess = FALSE;
    if (message != NULL)
    {
        printf(message);
        retSuccess = TRUE;
    }
    else
    {
        printf("ERROR: MESSAGE NOT VALID\n");
    }
    return retSuccess;
}

/* This functions calculates and returns the product of sums
    of the elements of three vectors (sum[i] = a[i] + b[i] + c[i]).
    The number of sums is equal to min(size,DIM). */    
uint32_t prodSum(uint32_t* v1, uint32_t v2[], uint32_t v3[DIM], uint32_t size)
{
    uint32_t retVal = 1U;
    /* Check if vectors are valid or not. */
    if ((v1 != NULL) && (v2 != NULL) && (v3 != NULL))
    {
        uint32_t realSize = size;
        if (size > DIM)
        {
            realSize = DIM;
        }
        uint32_t sumVect[realSize];
        uint32_t i;
        for (i = 0U; i < realSize; i++)
        {
            sumVect[i] = v1[i] + v2[i] + v3[i];
            retVal = retVal * sumVect[i];
        }
    }
    else
    {
        retVal = 0U;
    }
    return retVal;
}

bool_t isNegativeNumber(int32_t num)
{
  bool_t isNegative = FALSE;
  if (num < 0)
    {
      isNegative = TRUE;
    }
  return isNegative;
}

bool_t isWrittenToFile(int32_t fd, void* buff, uint32_t size)
{
  bool_t isWritten = FALSE;
  if (buff != NULL)
    {
      write(fd, buff, size);
      isWritten = TRUE;
    }
  return isWritten;
}

static void updateValue(uint32_t* value, uint32_t newVal)
{
  if (value != NULL)
    {
      *value = newVal;
    }
}

void updateVectorValues(uint32_t* vect, uint32_t val, uint32_t size)
{
  uint32_t i;

  if (vect != NULL)
    {
      for (i = 0U; i < size; i++)
	{
	  updateValue(&vect[i], i * val);
	}
    }
}

uint32_t returnGlobalPtrValue()
{
  uint32_t retVal;
  retVal = *globalPtr;
  return retVal;
}

static void printErrorMsg(char* msg, uint32_t size)
{
  write(stdout, msg, size);
  //printf("Printed Error Msg\n");
  //fflush(stdout);
}

void call_exit(uint32_t code)
{
  exit(code);
  //return;
}

void printError(void)
{
  printErrorMsg("ERROR",64);
}

uint32_t check_exit(uint32_t* ptr)
{
  struct timeval tv;
  int retVal = gettimeofday(&tv, NULL);
  char buffMsg[64U] = "[ ";
  char timeMsg[64U];
  snprintf(&timeMsg[0U], sizeof(timeMsg), "%d", tv.tv_sec);
  strcat(&buffMsg[0U], &timeMsg[0U]);
  strcat(&buffMsg[0U], " , ");
  snprintf(&timeMsg[0U], sizeof(timeMsg), "%d", tv.tv_sec);
  strcat(&buffMsg[0U], " ] ");
  strcat(&buffMsg[0U], "##ERROR: found a null ptr!!!##\n");
  if (ptr == NULL)
    {
      printErrorMsg(&buffMsg[0U], 64U);
      call_exit(0U);
    }
  return 1U;
}

uint32_t checkPtrIntegrity(uint32_t* ptr)
{
  uint32_t retVal = 1U;
  CHECK_EXIT(ptr);
  *ptr = 17U;
  return retVal;
}

uint32_t retNegFormat(format_t format)
{
  return ~(uint32_t)format;
}

format_t getGlobalFormat()
{
  return globalFormat;
}

void setGlobalCode(code_t code)
{
  globalCode = code;
}

code_t getGlobalCode()
{
  return globalCode;
}

void retDefInfoStruct(infoStruct_t** ptr)
{
  if (ptr!= NULL)
    {
      *ptr = &defInfoStruct;
    }
}

void retInfoStructEntry(infoStruct_t** ptr, uint32_t index)
{
  bool_t ok = TRUE;
  if (ptr == NULL)
    {
      ok = FALSE;
    }
  if (index >= DIM)
    {
      ok = FALSE;
    }
  if (ok == TRUE)
    {
      *ptr = &infoStructArray[index];
    }
}

bool_t setFirstInfoStructEntry(infoStruct_t** ptr)
{
  bool_t success = TRUE;
  if (ptr != NULL)
    {
      if (*ptr != NULL)
	{
	  infoStructArray[0U].key = (*ptr)->key;
	  infoStructArray[0U].vectorStruct->id = ((*ptr)->vectorStruct)->id;
	}
      else
	{
	  success = FALSE;
	}
    }
  else
    {
      success = FALSE;
    }
  return success;
}

void initInfoStructEntry(infoStruct_t* infoStrPtr, uint32_t index)
{
  vectStruct_t tmpVectStr;
  if (index < DIM)
    {
      infoStrPtr = &infoStructArray[index];
      infoStrPtr->key = 0U;
      tmpVectStr = initVectStruct();
      infoStrPtr->vectorStruct = &tmpVectStr;
    }
}

bool_t validInfoStructPtr(infoStruct_t* infoStrPtr)
{
  bool_t valid = FALSE;
  if (infoStrPtr != NULL)
    {
      if (infoStrPtr->vectorStruct != NULL)
	{
	  valid = TRUE;
	}
    }
  return valid;
}

bool_t isValidDataStruct(dataStruct_t* data)
{
  bool_t isValid = TRUE;

  uint32_t i;

  if (data != NULL)
    {
      if (data->format == FMT_NULL)
	{
	  isValid = FALSE;
	}
      if (((data->id == NULL) || (data->id[0] = '\0')) && isValid)
	{
	  isValid = FALSE;
	}
      if ((data->buffStruct.valid == INVALID) && isValid)
	{
	  isValid = FALSE;
	}
      for (i = 0U; i < BUFF_SIZE && isValid; i++)
	{
	  if (data->buffStruct.buff[i] == 0xDEADU)
	    {
	      isValid = FALSE;
	    }
	}
    }
  else
    {
      printf("Warning: passed a null ptr!\n");
      isValid = FALSE;
    }
  return isValid;
}

void getEmptyDataStruct(dataStruct_t* data)
{
  char tmpId = '\0';

  uint32_t i;

  if (data != NULL)
    {
      data->format = FMT_NULL;
      memcpy(data->id, &tmpId, sizeof(tmpId));
      data->buffStruct.valid = INVALID;
      
      for (i = 0U; i < BUFF_SIZE; i++)
	{
	  data->buffStruct.buff[i] = 0xDEADU;
	}
    }
}

void getEmptySetStruct(setStruct_t* set)
{
  if (set != NULL)
    {
      set->flag = FALSE;

      if (set->data != NULL)
	{
	  getEmptyDataStruct(set->data);
	}
    }
}

void retGlobalSet(setStruct_t* set)
{
  if (set != NULL)
    {
      memcpy(set, &globalSet, sizeof(globalSet));
    }
}

void setGlobalSet(setStruct_t set)
{
  memcpy(&globalSet, &set, sizeof(set));
}

void fillOnesArray(uint32_t array[3])
{
  uint32_t i;
  for (i = 0U; i < 3U; i++)
    {
      array[i] = 1U;
    }
}

void printCharArray(char array[DIM])
{
  printf("Array: %s\n",array);
}

void setMatrix(uint32_t** ptr)
{
  if (ptr != NULL)
    {
      if (*ptr != NULL)
	{
	  memcpy(&matrix[0U], *ptr, sizeof(matrix));
	}
    }
}

void getFirstEntryMatrix(uint32_t* ptr)
{
  if (ptr != NULL)
    {
      memcpy(ptr, &matrix[0U], sizeof(matrix[0U]));
    }
}

void setGlobalBuffStruct(buffStruct_t buffStruct)
{
  memcpy(globalBuffStruct, &buffStruct, sizeof(buffStruct));
}

void getGlobalBuffStuct(buffStruct_t* buffStruct)
{
  if (buffStruct != NULL)
    {
      memcpy(buffStruct, globalBuffStruct, sizeof(*buffStruct));
    }
}

void getGlobalArraySet(setArrayStruct_t* ptr)
{
  if (ptr != NULL)
    {
      memcpy(ptr, &globalArraySet, sizeof(globalArraySet));
    }
}

void copyArray(uint32_t outArray[3U], uint32_t inArray[3U])
{
  uint32_t i;
  for (i = 0U; i < 3U; i++)
    {
      outArray[i] = inArray[i];
    }
}

bool_t isZeroArray(array_t array)
{
  bool_t ret = TRUE;
  uint32_t i;
  for (i = 0U; i < 2U && !ret; i++)
    {
      if (array[i] != 0U)
	{
	  ret = FALSE;
	}
    }
  return ret;
}

void copyCharArray(char* out, char* in, uint32_t size)
{
  uint32_t i;
  for (i = 0U; i < size; i++)
    {
      out[i] = in[i];
    }
}

void copyCharArrayTwice(char* out, char* in_1, char* in_2, uint32_t size_1, uint32_t size_2)
{
  copyCharArray(out, in_1, size_1);
  copyCharArray(&out[size_1], in_2, size_2);
}

void getGlobalPair(pair_t* pair)
{
  if (pair != NULL)
    {
      memcpy(pair, &globalPair, sizeof(globalPair));
    }
}

void setGlobalPair(pair_t* pair)
{
  if (pair != NULL)
    {
      memcpy(&globalPair, pair, sizeof(globalPair));
    }
}

void copyPair(pair_t pair)
{
  memcpy(&globalPair, &pair, sizeof(pair));
}

void copyEntry(entry_t entry)
{
  memcpy(&globalEntry, &entry, sizeof(entry));
}

uint32_t countCharOcc(char msg[64], char c)
{
  uint32_t counter = 0U;
  uint32_t i;
  for (i = 0U; i < 64U; i++)
    {
      if (msg[i] == c)
	{
	  counter++;
	}
    }
  return counter;
}

bool_t setDefDataStruct(dataInfo_t dataInfo)
{
  bool_t success = TRUE;
  memcpy(&globalData, &dataInfo, sizeof(dataInfo));
  return success;
}

dataInfo_t getDefDataStruct()
{
  return globalData;
}

static void logg(char* message)
{
  printErrorMsg(message, 256);
}

void errorLog(char *msg) 
{
  logg(msg);
}

bool_t assertCheck(void* ptr, char* msg, char* file, int line, uint32_t checkVal)
{
  bool_t res = TRUE;
  if (ptr == NULL)
    {
      errorLog(msg);
      res = FALSE;
    }
  return res;
}

bool_t checkPtrs(void* ptr_1, void* ptr_2, void* ptr_3, char* file, int line, uint32_t checkVal)
{
  bool_t res;
  bool_t res_1;
  bool_t res_2;
  bool_t res_3;
  char msg[256] = "#abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ#@$%~()*&^ FATAL ERROR!!!!!! \n\t\r\n\nFOUND A NULL POINTER!!! SYSTEM ENTERS HALT STATE!!!! ########--/ERROR nfgkl1234567890`!@#$%^()*-_=+|{}{}[];;;nvmhfahu###\n";
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  /* Call again assertCheck */
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  /* Once more */
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res = (res_1 && res_2 && res_3);
  return res;
}

bool_t checkDataStructId(void)
{
  bool_t retVal = TRUE;
  if (globalDataStruct.id == NULL)
    {
      retVal = FALSE;
    }
  return retVal;
}

void getGlobalSetPtr(setStruct_t** ptr)
{
  if (ptr != NULL)
    {
      *ptr = globalSetPtr;
    }
}

/* Main program */
int main()
{
    uint32_t num = 10U;
    uint32_t retNum;
    uint32_t negNum = negVal(num, &retNum);
    uint32_t v1[] = {1, 2, 3};
    uint32_t v2[] = {4, 5, 6};
    uint32_t v3[DIM] = {7, 8, 9, 10, 11};
    uint32_t size = 3U;
    uint32_t retProdSum = prodSum(&v1[0U], v2, v3, size);
    printf("ProdSum value: %d\n",retProdSum);
    vectStruct_t v = initVectStruct();
    uint32_t i;
        
    printf("INITIALIZATION\n");
    for (i = 0U; i < DIM; i++)
    {
        printf("v.vector[%d]: %d\n",i,v.vector[i]);
    }
    printf("UPDATE\n");
    bool_t updated = FALSE;
    updateVectStruct(&v, 255U, &v3[0U],&updated);
    for (i = 0U; i < DIM; i++)
    {
        printf("v.vector[%d]: %d\n",i,v.vector[i]);
    }
    printf("updated: %d\n",updated);
    infoStruct_t infoStr;
    infoStr.key = 1U;
    infoStr.vectorStruct = &v;
    bool_t success = testInfoStruct(&infoStr, testVectStruct);
    printf("Info structure test result: %d\n",success);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "interface.h"
#include "types.h"
#include "utils.h"

/* MACRO DEFINITION */

#define DIM 5U
#define NB_OF_STR 3
#define BUFF_SIZE 64U

#define CHECK_EXIT(ptr) \
  do {		        \
    check_exit(ptr);    \
  } while (0)

/* END MACRO */

/* TYPEDEFS */
typedef uint32_t array_t[2];

array_t dummy_dummy;

typedef enum
  {
    FMT_NULL,
    FMT_DEC,
    FMT_CHAR
  } format_t;

typedef enum
  {
    CODE_0=255,
    CODE_1=1,
    CODE_2
  } code_t;

struct vectStruct_t
{
    uint32_t id;
    uint32_t vector[DIM];
    bool_t init;
};

typedef struct vectStruct_t vectStruct_t;

struct infoStruct_t
{
    uint32_t key;
    vectStruct_t* vectorStruct;
};

typedef struct infoStruct_t infoStruct_t;

typedef enum
{
    VALID,
    INVALID
} valid_t;

struct buffStruct_t
{
  valid_t valid;
  uint32_t buff[BUFF_SIZE];
};

typedef struct buffStruct_t buffStruct_t;

struct dataStruct_t
{
  format_t format;
  char* id;
  buffStruct_t buffStruct;
};

typedef struct dataStruct_t dataStruct_t;

struct setStruct_t
{
  bool_t flag;
  dataStruct_t* data;
};

typedef struct setStruct_t setStruct_t;

struct setArrayStruct_t
{
  uint32_t setArraySize;
  setStruct_t setArray[DIM];
};

typedef struct setArrayStruct_t setArrayStruct_t;

struct dataInfo_t
{
  dataStruct_t data[DIM];
  uint32_t nb_of_data;
  uint32_t current;
};

typedef struct dataInfo_t dataInfo_t;
/* END TYPEDEFS */

/* STATIC VARIABLES */
static bool_t nullBool = FALSE;
static uint32_t globalVal;
static uint32_t* globalPtr = NULL;
infoStruct_t infoStructArray[DIM];
infoStruct_t defInfoStruct;
static format_t globalFormat = FMT_NULL;
static setStruct_t globalSet;
code_t globalCode;
uint32_t matrix[DIM][DIM];
static buffStruct_t* globalBuffStruct;
static setArrayStruct_t globalArraySet;
static pair_t globalPair;
static entry_t globalEntry;
static dataStruct_t globalDataStruct;
static dataInfo_t globalData;
static setStruct_t* globalSetPtr;
/*END STATIC VARIABLES */

/* This function initializes a vector: it sets the value of vect[i] as i + 1, with i=1...DIM */
void initVector(uint32_t* vectPtr)
{
    uint32_t i;
    if (vectPtr != NULL)
    {
        for (i = 0U; i < DIM; i++)
        {
            vectPtr[i] = INCR_VAL(i);
        }
    }
}

void segFaultVector(uint32_t* vectPtr)
{
    uint32_t i;
    vectPtr = 0;
    i = *vectPtr;
}
/* This function returns a vector structure.
    Its fields are filled with default values. */
vectStruct_t initVectStruct()
{
    vectStruct_t retStr;
    uint32_t i;
    retStr.id = 0U;
    retStr.init = FALSE;
    for (i = 0U; i < DIM; i++)
    {
        retStr.vector[i] = 0U;
    }
    return retStr;
}

bool_t checkVectStruct(vectStruct_t vectStr)
{
  bool_t isUpdated = FALSE;

  if (vectStr.init == TRUE)
    {
      isUpdated = TRUE;
      printf("VectStr has already been updated... id = %d\n", vectStr.id);
    }
  else
    {
      printf("VectStr not updated\n");
    }
  
  return isUpdated;
}

/* This function updates a vector structure
    with the values contained in a vectPtr.
    Furthermore, it updates the init flag
    (TRUE) and the id value.    */
void updateVectStruct(vectStruct_t* vectStr, uint32_t id, uint32_t* vectPtr, bool_t* updated)
{
    if (updated != NULL)
    {
        *updated = FALSE;
        /* Check if pointers are valid or not. */
        if ((vectStr != NULL) && (vectPtr != NULL))
        {
            vectStr->id = id;
            uint32_t i;
            for (i = 0U; i < DIM; i++)
            {
                vectStr->vector[i] = *(vectPtr+i);
            }
            vectStr->init = TRUE;
            *updated = TRUE;
        }
    }
    else
    {
        nullBool = TRUE;
    }
}

/* This function checks if the info structure array is valid (i.e.
    each info structure has a not null vector structure pointer)
    and return an array of valid/invalid values. */
valid_t* checkValidInfoStruct(infoStruct_t infoStrArray[NB_OF_STR])
{
    valid_t* validArray = (valid_t*)malloc(sizeof(valid_t) * NB_OF_STR);
    uint32_t i;
    for (i = 0U; i < NB_OF_STR; i++)
    {
        if (infoStrArray[i].vectorStruct != NULL)
        {
            validArray[i] = VALID;
        }
        else
        {
            validArray[i] = INVALID;
        }
    }
    return validArray;
}

/* This function returns the number of elements in
    an array of vector structures equal to the given
    element. */
uint32_t countOccVectStructArray(vectStruct_t vectStrArray[NB_OF_STR], 
                                                 uint32_t elem)
{
    uint32_t nb_of_occ = 0U;
    uint32_t i;
    uint32_t j;
    for (i = 0U; i < NB_OF_STR; i++)
    {
        for (j = 0U; j < DIM; j++)
        {
            if (vectStrArray[i].vector[j] == elem)
            {
                nb_of_occ++;
            }
        }
    }
    return nb_of_occ;
}

/* This function checks if the given vector structure
    has at least one field different from zero and, in
    case, returns 1; otherwise, it returns 0. */
static uint32_t testVectStruct(vectStruct_t* vectStruct)
{
    uint32_t retVal = 0U;
    uint32_t i;
    uint32_t count = 0U;
    /* Check if pointer is valid or not. */
    if (vectStruct != NULL)
    {
        for (i = 0U; i < DIM; i++)
        {
            if (vectStruct->vector[i] != 0U)
            {
                count++;
            }
        }
        if (count != 0U)
        {
            retVal = 1U;
        }
    }
    return retVal;
}

/* This function checks if the vector structure is NULL. */
static uint32_t testNullStruct(vectStruct_t* vectStruct)
{
    uint32_t retVal;
    if (vectStruct == NULL)
    {
        retVal = 1U;
    }
    else
    {
        retVal = 0U;
    }
    return retVal;
}

/* This function checks the given info structure by using
    the given test function and returns TRUE if the test
    passes, otherwise FALSE. */
bool_t testInfoStruct(infoStruct_t* infoStruct, uint32_t (* funct)(vectStruct_t* vectStruct))
{
    bool_t success = FALSE;
    uint32_t retVal = 0U;
    /* Check if pointer is valid or not. */
    if ((infoStruct != NULL) && (funct != NULL))
    {
        /* If key == 0, the check fails */
        if (infoStruct->key != 0U)
        {
            retVal = (*funct)(infoStruct->vectorStruct);
            if (retVal == 1U)
            {
                success = TRUE;
            }
        }
    }
    return success;
}

/* This function fills a vector with the negated values of the given number. */
void fillNegVect(uint32_t* vect, uint32_t num)
{
    uint32_t dummy;
    uint32_t negNum;
    uint32_t i;
    
    /* Check if vector is valid or not. */
    if (vect != NULL)
    {
        negNum = negVal(num, &dummy);
        
        for (i = 0U; i < DIM; i++)
        {
            vect[i] = negNum;
        }
        
    }
    
}

static void copyVector(uint32_t* outVect, uint32_t* inVect, uint32_t size)
{
    uint32_t i;
    uint32_t tmpSize = size;
    
    if ((inVect != NULL) && (outVect != NULL))
    {
        if (size > DIM)
        {
            tmpSize = DIM;
        }
        
        for (i = 0U; i < tmpSize; i++)
        {
            outVect[i] = inVect[i];
        }
    }
}

uint32_t returnPtrVal(uint32_t* numPtr, uint32_t val)
{
    if (numPtr != NULL)
    {
        *numPtr = val;
    }
    return val;
}

/* This function */
void buildNegValVector(uint32_t* vectPtr)
{
    uint32_t i;
    uint32_t dummy;
    uint32_t tmpVal;    
    uint32_t simpleVect[DIM] = {1U, 2U, 3U, 4U, 5U};

    if (vectPtr != NULL)
    {
        copyVector(vectPtr, &simpleVect[0U], DIM);
        for (i = 0U; i < DIM; i++)
        {
            tmpVal = negVal(*(vectPtr + i), &dummy);
            vectPtr[i] = tmpVal;
        }
    }
}

/* This function prints a message */
bool_t printMsg(char* message)
{
    bool_t retSuccess = FALSE;
    if (message != NULL)
    {
        printf(message);
        retSuccess = TRUE;
    }
    else
    {
        printf("ERROR: MESSAGE NOT VALID\n");
    }
    return retSuccess;
}

/* This functions calculates and returns the product of sums
    of the elements of three vectors (sum[i] = a[i] + b[i] + c[i]).
    The number of sums is equal to min(size,DIM). */    
uint32_t prodSum(uint32_t* v1, uint32_t v2[], uint32_t v3[DIM], uint32_t size)
{
    uint32_t retVal = 1U;
    /* Check if vectors are valid or not. */
    if ((v1 != NULL) && (v2 != NULL) && (v3 != NULL))
    {
        uint32_t realSize = size;
        if (size > DIM)
        {
            realSize = DIM;
        }
        uint32_t sumVect[realSize];
        uint32_t i;
        for (i = 0U; i < realSize; i++)
        {
            sumVect[i] = v1[i] + v2[i] + v3[i];
            retVal = retVal * sumVect[i];
        }
    }
    else
    {
        retVal = 0U;
    }
    return retVal;
}

bool_t isNegativeNumber(int32_t num)
{
  bool_t isNegative = FALSE;
  if (num < 0)
    {
      isNegative = TRUE;
    }
  return isNegative;
}

bool_t isWrittenToFile(int32_t fd, void* buff, uint32_t size)
{
  bool_t isWritten = FALSE;
  if (buff != NULL)
    {
      write(fd, buff, size);
      isWritten = TRUE;
    }
  return isWritten;
}

static void updateValue(uint32_t* value, uint32_t newVal)
{
  if (value != NULL)
    {
      *value = newVal;
    }
}

void updateVectorValues(uint32_t* vect, uint32_t val, uint32_t size)
{
  uint32_t i;

  if (vect != NULL)
    {
      for (i = 0U; i < size; i++)
	{
	  updateValue(&vect[i], i * val);
	}
    }
}

uint32_t returnGlobalPtrValue()
{
  uint32_t retVal;
  retVal = *globalPtr;
  return retVal;
}

static void printErrorMsg(char* msg, uint32_t size)
{
  write(stdout, msg, size);
  //printf("Printed Error Msg\n");
  //fflush(stdout);
}

void call_exit(uint32_t code)
{
  exit(code);
  //return;
}

void printError(void)
{
  printErrorMsg("ERROR",64);
}

uint32_t check_exit(uint32_t* ptr)
{
  struct timeval tv;
  int retVal = gettimeofday(&tv, NULL);
  char buffMsg[64U] = "[ ";
  char timeMsg[64U];
  snprintf(&timeMsg[0U], sizeof(timeMsg), "%d", tv.tv_sec);
  strcat(&buffMsg[0U], &timeMsg[0U]);
  strcat(&buffMsg[0U], " , ");
  snprintf(&timeMsg[0U], sizeof(timeMsg), "%d", tv.tv_sec);
  strcat(&buffMsg[0U], " ] ");
  strcat(&buffMsg[0U], "##ERROR: found a null ptr!!!##\n");
  if (ptr == NULL)
    {
      printErrorMsg(&buffMsg[0U], 64U);
      call_exit(0U);
    }
  return 1U;
}

uint32_t checkPtrIntegrity(uint32_t* ptr)
{
  uint32_t retVal = 1U;
  CHECK_EXIT(ptr);
  *ptr = 17U;
  return retVal;
}

uint32_t retNegFormat(format_t format)
{
  return ~(uint32_t)format;
}

format_t getGlobalFormat()
{
  return globalFormat;
}

void setGlobalCode(code_t code)
{
  globalCode = code;
}

code_t getGlobalCode()
{
  return globalCode;
}

void retDefInfoStruct(infoStruct_t** ptr)
{
  if (ptr!= NULL)
    {
      *ptr = &defInfoStruct;
    }
}

void retInfoStructEntry(infoStruct_t** ptr, uint32_t index)
{
  bool_t ok = TRUE;
  if (ptr == NULL)
    {
      ok = FALSE;
    }
  if (index >= DIM)
    {
      ok = FALSE;
    }
  if (ok == TRUE)
    {
      *ptr = &infoStructArray[index];
    }
}

bool_t setFirstInfoStructEntry(infoStruct_t** ptr)
{
  bool_t success = TRUE;
  if (ptr != NULL)
    {
      if (*ptr != NULL)
	{
	  infoStructArray[0U].key = (*ptr)->key;
	  infoStructArray[0U].vectorStruct->id = ((*ptr)->vectorStruct)->id;
	}
      else
	{
	  success = FALSE;
	}
    }
  else
    {
      success = FALSE;
    }
  return success;
}

void initInfoStructEntry(infoStruct_t* infoStrPtr, uint32_t index)
{
  vectStruct_t tmpVectStr;
  if (index < DIM)
    {
      infoStrPtr = &infoStructArray[index];
      infoStrPtr->key = 0U;
      tmpVectStr = initVectStruct();
      infoStrPtr->vectorStruct = &tmpVectStr;
    }
}

bool_t validInfoStructPtr(infoStruct_t* infoStrPtr)
{
  bool_t valid = FALSE;
  if (infoStrPtr != NULL)
    {
      if (infoStrPtr->vectorStruct != NULL)
	{
	  valid = TRUE;
	}
    }
  return valid;
}

bool_t isValidDataStruct(dataStruct_t* data)
{
  bool_t isValid = TRUE;

  uint32_t i;

  if (data != NULL)
    {
      if (data->format == FMT_NULL)
	{
	  isValid = FALSE;
	}
      if (((data->id == NULL) || (data->id[0] = '\0')) && isValid)
	{
	  isValid = FALSE;
	}
      if ((data->buffStruct.valid == INVALID) && isValid)
	{
	  isValid = FALSE;
	}
      for (i = 0U; i < BUFF_SIZE && isValid; i++)
	{
	  if (data->buffStruct.buff[i] == 0xDEADU)
	    {
	      isValid = FALSE;
	    }
	}
    }
  else
    {
      printf("Warning: passed a null ptr!\n");
      isValid = FALSE;
    }
  return isValid;
}

void getEmptyDataStruct(dataStruct_t* data)
{
  char tmpId = '\0';

  uint32_t i;

  if (data != NULL)
    {
      data->format = FMT_NULL;
      memcpy(data->id, &tmpId, sizeof(tmpId));
      data->buffStruct.valid = INVALID;
      
      for (i = 0U; i < BUFF_SIZE; i++)
	{
	  data->buffStruct.buff[i] = 0xDEADU;
	}
    }
}

void getEmptySetStruct(setStruct_t* set)
{
  if (set != NULL)
    {
      set->flag = FALSE;

      if (set->data != NULL)
	{
	  getEmptyDataStruct(set->data);
	}
    }
}

void retGlobalSet(setStruct_t* set)
{
  if (set != NULL)
    {
      memcpy(set, &globalSet, sizeof(globalSet));
    }
}

void setGlobalSet(setStruct_t set)
{
  memcpy(&globalSet, &set, sizeof(set));
}

void fillOnesArray(uint32_t array[3])
{
  uint32_t i;
  for (i = 0U; i < 3U; i++)
    {
      array[i] = 1U;
    }
}

void printCharArray(char array[DIM])
{
  printf("Array: %s\n",array);
}

void setMatrix(uint32_t** ptr)
{
  if (ptr != NULL)
    {
      if (*ptr != NULL)
	{
	  memcpy(&matrix[0U], *ptr, sizeof(matrix));
	}
    }
}

void getFirstEntryMatrix(uint32_t* ptr)
{
  if (ptr != NULL)
    {
      memcpy(ptr, &matrix[0U], sizeof(matrix[0U]));
    }
}

void setGlobalBuffStruct(buffStruct_t buffStruct)
{
  memcpy(globalBuffStruct, &buffStruct, sizeof(buffStruct));
}

void getGlobalBuffStuct(buffStruct_t* buffStruct)
{
  if (buffStruct != NULL)
    {
      memcpy(buffStruct, globalBuffStruct, sizeof(*buffStruct));
    }
}

void getGlobalArraySet(setArrayStruct_t* ptr)
{
  if (ptr != NULL)
    {
      memcpy(ptr, &globalArraySet, sizeof(globalArraySet));
    }
}

void copyArray(uint32_t outArray[3U], uint32_t inArray[3U])
{
  uint32_t i;
  for (i = 0U; i < 3U; i++)
    {
      outArray[i] = inArray[i];
    }
}

bool_t isZeroArray(array_t array)
{
  bool_t ret = TRUE;
  uint32_t i;
  for (i = 0U; i < 2U && !ret; i++)
    {
      if (array[i] != 0U)
	{
	  ret = FALSE;
	}
    }
  return ret;
}

void copyCharArray(char* out, char* in, uint32_t size)
{
  uint32_t i;
  for (i = 0U; i < size; i++)
    {
      out[i] = in[i];
    }
}

void copyCharArrayTwice(char* out, char* in_1, char* in_2, uint32_t size_1, uint32_t size_2)
{
  copyCharArray(out, in_1, size_1);
  copyCharArray(&out[size_1], in_2, size_2);
}

void getGlobalPair(pair_t* pair)
{
  if (pair != NULL)
    {
      memcpy(pair, &globalPair, sizeof(globalPair));
    }
}

void setGlobalPair(pair_t* pair)
{
  if (pair != NULL)
    {
      memcpy(&globalPair, pair, sizeof(globalPair));
    }
}

void copyPair(pair_t pair)
{
  memcpy(&globalPair, &pair, sizeof(pair));
}

void copyEntry(entry_t entry)
{
  memcpy(&globalEntry, &entry, sizeof(entry));
}

uint32_t countCharOcc(char msg[64], char c)
{
  uint32_t counter = 0U;
  uint32_t i;
  for (i = 0U; i < 64U; i++)
    {
      if (msg[i] == c)
	{
	  counter++;
	}
    }
  return counter;
}

bool_t setDefDataStruct(dataInfo_t dataInfo)
{
  bool_t success = TRUE;
  memcpy(&globalData, &dataInfo, sizeof(dataInfo));
  return success;
}

dataInfo_t getDefDataStruct()
{
  return globalData;
}

static void logg(char* message)
{
  printErrorMsg(message, 256);
}

void errorLog(char *msg) 
{
  logg(msg);
}

bool_t assertCheck(void* ptr, char* msg, char* file, int line, uint32_t checkVal)
{
  bool_t res = TRUE;
  if (ptr == NULL)
    {
      errorLog(msg);
      res = FALSE;
    }
  return res;
}

bool_t checkPtrs(void* ptr_1, void* ptr_2, void* ptr_3, char* file, int line, uint32_t checkVal)
{
  bool_t res;
  bool_t res_1;
  bool_t res_2;
  bool_t res_3;
  char msg[256] = "#abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ#@$%~()*&^ FATAL ERROR!!!!!! \n\t\r\n\nFOUND A NULL POINTER!!! SYSTEM ENTERS HALT STATE!!!! ########--/ERROR nfgkl1234567890`!@#$%^()*-_=+|{}{}[];;;nvmhfahu###\n";
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  /* Call again assertCheck */
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  /* Once more */
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res = (res_1 && res_2 && res_3);
  return res;
}

bool_t checkDataStructId(void)
{
  bool_t retVal = TRUE;
  if (globalDataStruct.id == NULL)
    {
      retVal = FALSE;
    }
  return retVal;
}

void getGlobalSetPtr(setStruct_t** ptr)
{
  if (ptr != NULL)
    {
      *ptr = globalSetPtr;
    }
}

/* Main program */
int main()
{
    uint32_t num = 10U;
    uint32_t retNum;
    uint32_t negNum = negVal(num, &retNum);
    uint32_t v1[] = {1, 2, 3};
    uint32_t v2[] = {4, 5, 6};
    uint32_t v3[DIM] = {7, 8, 9, 10, 11};
    uint32_t size = 3U;
    uint32_t retProdSum = prodSum(&v1[0U], v2, v3, size);
    printf("ProdSum value: %d\n",retProdSum);
    vectStruct_t v = initVectStruct();
    uint32_t i;
        
    printf("INITIALIZATION\n");
    for (i = 0U; i < DIM; i++)
    {
        printf("v.vector[%d]: %d\n",i,v.vector[i]);
    }
    printf("UPDATE\n");
    bool_t updated = FALSE;
    updateVectStruct(&v, 255U, &v3[0U],&updated);
    for (i = 0U; i < DIM; i++)
    {
        printf("v.vector[%d]: %d\n",i,v.vector[i]);
    }
    printf("updated: %d\n",updated);
    infoStruct_t infoStr;
    infoStr.key = 1U;
    infoStr.vectorStruct = &v;
    bool_t success = testInfoStruct(&infoStr, testVectStruct);
    printf("Info structure test result: %d\n",success);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "interface.h"
#include "types.h"
#include "utils.h"

/* MACRO DEFINITION */

#define DIM 5U
#define NB_OF_STR 3
#define BUFF_SIZE 64U

#define CHECK_EXIT(ptr) \
  do {		        \
    check_exit(ptr);    \
  } while (0)

/* END MACRO */

/* TYPEDEFS */
typedef uint32_t array_t[2];

array_t dummy_dummy;

typedef enum
  {
    FMT_NULL,
    FMT_DEC,
    FMT_CHAR
  } format_t;

typedef enum
  {
    CODE_0=255,
    CODE_1=1,
    CODE_2
  } code_t;

struct vectStruct_t
{
    uint32_t id;
    uint32_t vector[DIM];
    bool_t init;
};

typedef struct vectStruct_t vectStruct_t;

struct infoStruct_t
{
    uint32_t key;
    vectStruct_t* vectorStruct;
};

typedef struct infoStruct_t infoStruct_t;

typedef enum
{
    VALID,
    INVALID
} valid_t;

struct buffStruct_t
{
  valid_t valid;
  uint32_t buff[BUFF_SIZE];
};

typedef struct buffStruct_t buffStruct_t;

struct dataStruct_t
{
  format_t format;
  char* id;
  buffStruct_t buffStruct;
};

typedef struct dataStruct_t dataStruct_t;

struct setStruct_t
{
  bool_t flag;
  dataStruct_t* data;
};

typedef struct setStruct_t setStruct_t;

struct setArrayStruct_t
{
  uint32_t setArraySize;
  setStruct_t setArray[DIM];
};

typedef struct setArrayStruct_t setArrayStruct_t;

struct dataInfo_t
{
  dataStruct_t data[DIM];
  uint32_t nb_of_data;
  uint32_t current;
};

typedef struct dataInfo_t dataInfo_t;
/* END TYPEDEFS */

/* STATIC VARIABLES */
static bool_t nullBool = FALSE;
static uint32_t globalVal;
static uint32_t* globalPtr = NULL;
infoStruct_t infoStructArray[DIM];
infoStruct_t defInfoStruct;
static format_t globalFormat = FMT_NULL;
static setStruct_t globalSet;
code_t globalCode;
uint32_t matrix[DIM][DIM];
static buffStruct_t* globalBuffStruct;
static setArrayStruct_t globalArraySet;
static pair_t globalPair;
static entry_t globalEntry;
static dataStruct_t globalDataStruct;
static dataInfo_t globalData;
static setStruct_t* globalSetPtr;
/*END STATIC VARIABLES */

/* This function initializes a vector: it sets the value of vect[i] as i + 1, with i=1...DIM */
void initVector(uint32_t* vectPtr)
{
    uint32_t i;
    if (vectPtr != NULL)
    {
        for (i = 0U; i < DIM; i++)
        {
            vectPtr[i] = INCR_VAL(i);
        }
    }
}

void segFaultVector(uint32_t* vectPtr)
{
    uint32_t i;
    vectPtr = 0;
    i = *vectPtr;
}
/* This function returns a vector structure.
    Its fields are filled with default values. */
vectStruct_t initVectStruct()
{
    vectStruct_t retStr;
    uint32_t i;
    retStr.id = 0U;
    retStr.init = FALSE;
    for (i = 0U; i < DIM; i++)
    {
        retStr.vector[i] = 0U;
    }
    return retStr;
}

bool_t checkVectStruct(vectStruct_t vectStr)
{
  bool_t isUpdated = FALSE;

  if (vectStr.init == TRUE)
    {
      isUpdated = TRUE;
      printf("VectStr has already been updated... id = %d\n", vectStr.id);
    }
  else
    {
      printf("VectStr not updated\n");
    }
  
  return isUpdated;
}

/* This function updates a vector structure
    with the values contained in a vectPtr.
    Furthermore, it updates the init flag
    (TRUE) and the id value.    */
void updateVectStruct(vectStruct_t* vectStr, uint32_t id, uint32_t* vectPtr, bool_t* updated)
{
    if (updated != NULL)
    {
        *updated = FALSE;
        /* Check if pointers are valid or not. */
        if ((vectStr != NULL) && (vectPtr != NULL))
        {
            vectStr->id = id;
            uint32_t i;
            for (i = 0U; i < DIM; i++)
            {
                vectStr->vector[i] = *(vectPtr+i);
            }
            vectStr->init = TRUE;
            *updated = TRUE;
        }
    }
    else
    {
        nullBool = TRUE;
    }
}

/* This function checks if the info structure array is valid (i.e.
    each info structure has a not null vector structure pointer)
    and return an array of valid/invalid values. */
valid_t* checkValidInfoStruct(infoStruct_t infoStrArray[NB_OF_STR])
{
    valid_t* validArray = (valid_t*)malloc(sizeof(valid_t) * NB_OF_STR);
    uint32_t i;
    for (i = 0U; i < NB_OF_STR; i++)
    {
        if (infoStrArray[i].vectorStruct != NULL)
        {
            validArray[i] = VALID;
        }
        else
        {
            validArray[i] = INVALID;
        }
    }
    return validArray;
}

/* This function returns the number of elements in
    an array of vector structures equal to the given
    element. */
uint32_t countOccVectStructArray(vectStruct_t vectStrArray[NB_OF_STR], 
                                                 uint32_t elem)
{
    uint32_t nb_of_occ = 0U;
    uint32_t i;
    uint32_t j;
    for (i = 0U; i < NB_OF_STR; i++)
    {
        for (j = 0U; j < DIM; j++)
        {
            if (vectStrArray[i].vector[j] == elem)
            {
                nb_of_occ++;
            }
        }
    }
    return nb_of_occ;
}

/* This function checks if the given vector structure
    has at least one field different from zero and, in
    case, returns 1; otherwise, it returns 0. */
static uint32_t testVectStruct(vectStruct_t* vectStruct)
{
    uint32_t retVal = 0U;
    uint32_t i;
    uint32_t count = 0U;
    /* Check if pointer is valid or not. */
    if (vectStruct != NULL)
    {
        for (i = 0U; i < DIM; i++)
        {
            if (vectStruct->vector[i] != 0U)
            {
                count++;
            }
        }
        if (count != 0U)
        {
            retVal = 1U;
        }
    }
    return retVal;
}

/* This function checks if the vector structure is NULL. */
static uint32_t testNullStruct(vectStruct_t* vectStruct)
{
    uint32_t retVal;
    if (vectStruct == NULL)
    {
        retVal = 1U;
    }
    else
    {
        retVal = 0U;
    }
    return retVal;
}

/* This function checks the given info structure by using
    the given test function and returns TRUE if the test
    passes, otherwise FALSE. */
bool_t testInfoStruct(infoStruct_t* infoStruct, uint32_t (* funct)(vectStruct_t* vectStruct))
{
    bool_t success = FALSE;
    uint32_t retVal = 0U;
    /* Check if pointer is valid or not. */
    if ((infoStruct != NULL) && (funct != NULL))
    {
        /* If key == 0, the check fails */
        if (infoStruct->key != 0U)
        {
            retVal = (*funct)(infoStruct->vectorStruct);
            if (retVal == 1U)
            {
                success = TRUE;
            }
        }
    }
    return success;
}

/* This function fills a vector with the negated values of the given number. */
void fillNegVect(uint32_t* vect, uint32_t num)
{
    uint32_t dummy;
    uint32_t negNum;
    uint32_t i;
    
    /* Check if vector is valid or not. */
    if (vect != NULL)
    {
        negNum = negVal(num, &dummy);
        
        for (i = 0U; i < DIM; i++)
        {
            vect[i] = negNum;
        }
        
    }
    
}

static void copyVector(uint32_t* outVect, uint32_t* inVect, uint32_t size)
{
    uint32_t i;
    uint32_t tmpSize = size;
    
    if ((inVect != NULL) && (outVect != NULL))
    {
        if (size > DIM)
        {
            tmpSize = DIM;
        }
        
        for (i = 0U; i < tmpSize; i++)
        {
            outVect[i] = inVect[i];
        }
    }
}

uint32_t returnPtrVal(uint32_t* numPtr, uint32_t val)
{
    if (numPtr != NULL)
    {
        *numPtr = val;
    }
    return val;
}

/* This function */
void buildNegValVector(uint32_t* vectPtr)
{
    uint32_t i;
    uint32_t dummy;
    uint32_t tmpVal;    
    uint32_t simpleVect[DIM] = {1U, 2U, 3U, 4U, 5U};

    if (vectPtr != NULL)
    {
        copyVector(vectPtr, &simpleVect[0U], DIM);
        for (i = 0U; i < DIM; i++)
        {
            tmpVal = negVal(*(vectPtr + i), &dummy);
            vectPtr[i] = tmpVal;
        }
    }
}

/* This function prints a message */
bool_t printMsg(char* message)
{
    bool_t retSuccess = FALSE;
    if (message != NULL)
    {
        printf(message);
        retSuccess = TRUE;
    }
    else
    {
        printf("ERROR: MESSAGE NOT VALID\n");
    }
    return retSuccess;
}

/* This functions calculates and returns the product of sums
    of the elements of three vectors (sum[i] = a[i] + b[i] + c[i]).
    The number of sums is equal to min(size,DIM). */    
uint32_t prodSum(uint32_t* v1, uint32_t v2[], uint32_t v3[DIM], uint32_t size)
{
    uint32_t retVal = 1U;
    /* Check if vectors are valid or not. */
    if ((v1 != NULL) && (v2 != NULL) && (v3 != NULL))
    {
        uint32_t realSize = size;
        if (size > DIM)
        {
            realSize = DIM;
        }
        uint32_t sumVect[realSize];
        uint32_t i;
        for (i = 0U; i < realSize; i++)
        {
            sumVect[i] = v1[i] + v2[i] + v3[i];
            retVal = retVal * sumVect[i];
        }
    }
    else
    {
        retVal = 0U;
    }
    return retVal;
}

bool_t isNegativeNumber(int32_t num)
{
  bool_t isNegative = FALSE;
  if (num < 0)
    {
      isNegative = TRUE;
    }
  return isNegative;
}

bool_t isWrittenToFile(int32_t fd, void* buff, uint32_t size)
{
  bool_t isWritten = FALSE;
  if (buff != NULL)
    {
      write(fd, buff, size);
      isWritten = TRUE;
    }
  return isWritten;
}

static void updateValue(uint32_t* value, uint32_t newVal)
{
  if (value != NULL)
    {
      *value = newVal;
    }
}

void updateVectorValues(uint32_t* vect, uint32_t val, uint32_t size)
{
  uint32_t i;

  if (vect != NULL)
    {
      for (i = 0U; i < size; i++)
	{
	  updateValue(&vect[i], i * val);
	}
    }
}

uint32_t returnGlobalPtrValue()
{
  uint32_t retVal;
  retVal = *globalPtr;
  return retVal;
}

static void printErrorMsg(char* msg, uint32_t size)
{
  write(stdout, msg, size);
  //printf("Printed Error Msg\n");
  //fflush(stdout);
}

void call_exit(uint32_t code)
{
  exit(code);
  //return;
}

void printError(void)
{
  printErrorMsg("ERROR",64);
}

uint32_t check_exit(uint32_t* ptr)
{
  struct timeval tv;
  int retVal = gettimeofday(&tv, NULL);
  char buffMsg[64U] = "[ ";
  char timeMsg[64U];
  snprintf(&timeMsg[0U], sizeof(timeMsg), "%d", tv.tv_sec);
  strcat(&buffMsg[0U], &timeMsg[0U]);
  strcat(&buffMsg[0U], " , ");
  snprintf(&timeMsg[0U], sizeof(timeMsg), "%d", tv.tv_sec);
  strcat(&buffMsg[0U], " ] ");
  strcat(&buffMsg[0U], "##ERROR: found a null ptr!!!##\n");
  if (ptr == NULL)
    {
      printErrorMsg(&buffMsg[0U], 64U);
      call_exit(0U);
    }
  return 1U;
}

uint32_t checkPtrIntegrity(uint32_t* ptr)
{
  uint32_t retVal = 1U;
  CHECK_EXIT(ptr);
  *ptr = 17U;
  return retVal;
}

uint32_t retNegFormat(format_t format)
{
  return ~(uint32_t)format;
}

format_t getGlobalFormat()
{
  return globalFormat;
}

void setGlobalCode(code_t code)
{
  globalCode = code;
}

code_t getGlobalCode()
{
  return globalCode;
}

void retDefInfoStruct(infoStruct_t** ptr)
{
  if (ptr!= NULL)
    {
      *ptr = &defInfoStruct;
    }
}

void retInfoStructEntry(infoStruct_t** ptr, uint32_t index)
{
  bool_t ok = TRUE;
  if (ptr == NULL)
    {
      ok = FALSE;
    }
  if (index >= DIM)
    {
      ok = FALSE;
    }
  if (ok == TRUE)
    {
      *ptr = &infoStructArray[index];
    }
}

bool_t setFirstInfoStructEntry(infoStruct_t** ptr)
{
  bool_t success = TRUE;
  if (ptr != NULL)
    {
      if (*ptr != NULL)
	{
	  infoStructArray[0U].key = (*ptr)->key;
	  infoStructArray[0U].vectorStruct->id = ((*ptr)->vectorStruct)->id;
	}
      else
	{
	  success = FALSE;
	}
    }
  else
    {
      success = FALSE;
    }
  return success;
}

void initInfoStructEntry(infoStruct_t* infoStrPtr, uint32_t index)
{
  vectStruct_t tmpVectStr;
  if (index < DIM)
    {
      infoStrPtr = &infoStructArray[index];
      infoStrPtr->key = 0U;
      tmpVectStr = initVectStruct();
      infoStrPtr->vectorStruct = &tmpVectStr;
    }
}

bool_t validInfoStructPtr(infoStruct_t* infoStrPtr)
{
  bool_t valid = FALSE;
  if (infoStrPtr != NULL)
    {
      if (infoStrPtr->vectorStruct != NULL)
	{
	  valid = TRUE;
	}
    }
  return valid;
}

bool_t isValidDataStruct(dataStruct_t* data)
{
  bool_t isValid = TRUE;

  uint32_t i;

  if (data != NULL)
    {
      if (data->format == FMT_NULL)
	{
	  isValid = FALSE;
	}
      if (((data->id == NULL) || (data->id[0] = '\0')) && isValid)
	{
	  isValid = FALSE;
	}
      if ((data->buffStruct.valid == INVALID) && isValid)
	{
	  isValid = FALSE;
	}
      for (i = 0U; i < BUFF_SIZE && isValid; i++)
	{
	  if (data->buffStruct.buff[i] == 0xDEADU)
	    {
	      isValid = FALSE;
	    }
	}
    }
  else
    {
      printf("Warning: passed a null ptr!\n");
      isValid = FALSE;
    }
  return isValid;
}

void getEmptyDataStruct(dataStruct_t* data)
{
  char tmpId = '\0';

  uint32_t i;

  if (data != NULL)
    {
      data->format = FMT_NULL;
      memcpy(data->id, &tmpId, sizeof(tmpId));
      data->buffStruct.valid = INVALID;
      
      for (i = 0U; i < BUFF_SIZE; i++)
	{
	  data->buffStruct.buff[i] = 0xDEADU;
	}
    }
}

void getEmptySetStruct(setStruct_t* set)
{
  if (set != NULL)
    {
      set->flag = FALSE;

      if (set->data != NULL)
	{
	  getEmptyDataStruct(set->data);
	}
    }
}

void retGlobalSet(setStruct_t* set)
{
  if (set != NULL)
    {
      memcpy(set, &globalSet, sizeof(globalSet));
    }
}

void setGlobalSet(setStruct_t set)
{
  memcpy(&globalSet, &set, sizeof(set));
}

void fillOnesArray(uint32_t array[3])
{
  uint32_t i;
  for (i = 0U; i < 3U; i++)
    {
      array[i] = 1U;
    }
}

void printCharArray(char array[DIM])
{
  printf("Array: %s\n",array);
}

void setMatrix(uint32_t** ptr)
{
  if (ptr != NULL)
    {
      if (*ptr != NULL)
	{
	  memcpy(&matrix[0U], *ptr, sizeof(matrix));
	}
    }
}

void getFirstEntryMatrix(uint32_t* ptr)
{
  if (ptr != NULL)
    {
      memcpy(ptr, &matrix[0U], sizeof(matrix[0U]));
    }
}

void setGlobalBuffStruct(buffStruct_t buffStruct)
{
  memcpy(globalBuffStruct, &buffStruct, sizeof(buffStruct));
}

void getGlobalBuffStuct(buffStruct_t* buffStruct)
{
  if (buffStruct != NULL)
    {
      memcpy(buffStruct, globalBuffStruct, sizeof(*buffStruct));
    }
}

void getGlobalArraySet(setArrayStruct_t* ptr)
{
  if (ptr != NULL)
    {
      memcpy(ptr, &globalArraySet, sizeof(globalArraySet));
    }
}

void copyArray(uint32_t outArray[3U], uint32_t inArray[3U])
{
  uint32_t i;
  for (i = 0U; i < 3U; i++)
    {
      outArray[i] = inArray[i];
    }
}

bool_t isZeroArray(array_t array)
{
  bool_t ret = TRUE;
  uint32_t i;
  for (i = 0U; i < 2U && !ret; i++)
    {
      if (array[i] != 0U)
	{
	  ret = FALSE;
	}
    }
  return ret;
}

void copyCharArray(char* out, char* in, uint32_t size)
{
  uint32_t i;
  for (i = 0U; i < size; i++)
    {
      out[i] = in[i];
    }
}

void copyCharArrayTwice(char* out, char* in_1, char* in_2, uint32_t size_1, uint32_t size_2)
{
  copyCharArray(out, in_1, size_1);
  copyCharArray(&out[size_1], in_2, size_2);
}

void getGlobalPair(pair_t* pair)
{
  if (pair != NULL)
    {
      memcpy(pair, &globalPair, sizeof(globalPair));
    }
}

void setGlobalPair(pair_t* pair)
{
  if (pair != NULL)
    {
      memcpy(&globalPair, pair, sizeof(globalPair));
    }
}

void copyPair(pair_t pair)
{
  memcpy(&globalPair, &pair, sizeof(pair));
}

void copyEntry(entry_t entry)
{
  memcpy(&globalEntry, &entry, sizeof(entry));
}

uint32_t countCharOcc(char msg[64], char c)
{
  uint32_t counter = 0U;
  uint32_t i;
  for (i = 0U; i < 64U; i++)
    {
      if (msg[i] == c)
	{
	  counter++;
	}
    }
  return counter;
}

bool_t setDefDataStruct(dataInfo_t dataInfo)
{
  bool_t success = TRUE;
  memcpy(&globalData, &dataInfo, sizeof(dataInfo));
  return success;
}

dataInfo_t getDefDataStruct()
{
  return globalData;
}

static void logg(char* message)
{
  printErrorMsg(message, 256);
}

void errorLog(char *msg) 
{
  logg(msg);
}

bool_t assertCheck(void* ptr, char* msg, char* file, int line, uint32_t checkVal)
{
  bool_t res = TRUE;
  if (ptr == NULL)
    {
      errorLog(msg);
      res = FALSE;
    }
  return res;
}

bool_t checkPtrs(void* ptr_1, void* ptr_2, void* ptr_3, char* file, int line, uint32_t checkVal)
{
  bool_t res;
  bool_t res_1;
  bool_t res_2;
  bool_t res_3;
  char msg[256] = "#abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ#@$%~()*&^ FATAL ERROR!!!!!! \n\t\r\n\nFOUND A NULL POINTER!!! SYSTEM ENTERS HALT STATE!!!! ########--/ERROR nfgkl1234567890`!@#$%^()*-_=+|{}{}[];;;nvmhfahu###\n";
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  /* Call again assertCheck */
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  /* Once more */
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res = (res_1 && res_2 && res_3);
  return res;
}

bool_t checkDataStructId(void)
{
  bool_t retVal = TRUE;
  if (globalDataStruct.id == NULL)
    {
      retVal = FALSE;
    }
  return retVal;
}

void getGlobalSetPtr(setStruct_t** ptr)
{
  if (ptr != NULL)
    {
      *ptr = globalSetPtr;
    }
}

/* Main program */
int main()
{
    uint32_t num = 10U;
    uint32_t retNum;
    uint32_t negNum = negVal(num, &retNum);
    uint32_t v1[] = {1, 2, 3};
    uint32_t v2[] = {4, 5, 6};
    uint32_t v3[DIM] = {7, 8, 9, 10, 11};
    uint32_t size = 3U;
    uint32_t retProdSum = prodSum(&v1[0U], v2, v3, size);
    printf("ProdSum value: %d\n",retProdSum);
    vectStruct_t v = initVectStruct();
    uint32_t i;
        
    printf("INITIALIZATION\n");
    for (i = 0U; i < DIM; i++)
    {
        printf("v.vector[%d]: %d\n",i,v.vector[i]);
    }
    printf("UPDATE\n");
    bool_t updated = FALSE;
    updateVectStruct(&v, 255U, &v3[0U],&updated);
    for (i = 0U; i < DIM; i++)
    {
        printf("v.vector[%d]: %d\n",i,v.vector[i]);
    }
    printf("updated: %d\n",updated);
    infoStruct_t infoStr;
    infoStr.key = 1U;
    infoStr.vectorStruct = &v;
    bool_t success = testInfoStruct(&infoStr, testVectStruct);
    printf("Info structure test result: %d\n",success);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "interface.h"
#include "types.h"
#include "utils.h"

/* MACRO DEFINITION */

#define DIM 5U
#define NB_OF_STR 3
#define BUFF_SIZE 64U

#define CHECK_EXIT(ptr) \
  do {		        \
    check_exit(ptr);    \
  } while (0)

/* END MACRO */

/* TYPEDEFS */
typedef uint32_t array_t[2];

array_t dummy_dummy;

typedef enum
  {
    FMT_NULL,
    FMT_DEC,
    FMT_CHAR
  } format_t;

typedef enum
  {
    CODE_0=255,
    CODE_1=1,
    CODE_2
  } code_t;

struct vectStruct_t
{
    uint32_t id;
    uint32_t vector[DIM];
    bool_t init;
};

typedef struct vectStruct_t vectStruct_t;

struct infoStruct_t
{
    uint32_t key;
    vectStruct_t* vectorStruct;
};

typedef struct infoStruct_t infoStruct_t;

typedef enum
{
    VALID,
    INVALID
} valid_t;

struct buffStruct_t
{
  valid_t valid;
  uint32_t buff[BUFF_SIZE];
};

typedef struct buffStruct_t buffStruct_t;

struct dataStruct_t
{
  format_t format;
  char* id;
  buffStruct_t buffStruct;
};

typedef struct dataStruct_t dataStruct_t;

struct setStruct_t
{
  bool_t flag;
  dataStruct_t* data;
};

typedef struct setStruct_t setStruct_t;

struct setArrayStruct_t
{
  uint32_t setArraySize;
  setStruct_t setArray[DIM];
};

typedef struct setArrayStruct_t setArrayStruct_t;

struct dataInfo_t
{
  dataStruct_t data[DIM];
  uint32_t nb_of_data;
  uint32_t current;
};

typedef struct dataInfo_t dataInfo_t;
/* END TYPEDEFS */

/* STATIC VARIABLES */
static bool_t nullBool = FALSE;
static uint32_t globalVal;
static uint32_t* globalPtr = NULL;
infoStruct_t infoStructArray[DIM];
infoStruct_t defInfoStruct;
static format_t globalFormat = FMT_NULL;
static setStruct_t globalSet;
code_t globalCode;
uint32_t matrix[DIM][DIM];
static buffStruct_t* globalBuffStruct;
static setArrayStruct_t globalArraySet;
static pair_t globalPair;
static entry_t globalEntry;
static dataStruct_t globalDataStruct;
static dataInfo_t globalData;
static setStruct_t* globalSetPtr;
/*END STATIC VARIABLES */

/* This function initializes a vector: it sets the value of vect[i] as i + 1, with i=1...DIM */
void initVector(uint32_t* vectPtr)
{
    uint32_t i;
    if (vectPtr != NULL)
    {
        for (i = 0U; i < DIM; i++)
        {
            vectPtr[i] = INCR_VAL(i);
        }
    }
}

void segFaultVector(uint32_t* vectPtr)
{
    uint32_t i;
    vectPtr = 0;
    i = *vectPtr;
}
/* This function returns a vector structure.
    Its fields are filled with default values. */
vectStruct_t initVectStruct()
{
    vectStruct_t retStr;
    uint32_t i;
    retStr.id = 0U;
    retStr.init = FALSE;
    for (i = 0U; i < DIM; i++)
    {
        retStr.vector[i] = 0U;
    }
    return retStr;
}

bool_t checkVectStruct(vectStruct_t vectStr)
{
  bool_t isUpdated = FALSE;

  if (vectStr.init == TRUE)
    {
      isUpdated = TRUE;
      printf("VectStr has already been updated... id = %d\n", vectStr.id);
    }
  else
    {
      printf("VectStr not updated\n");
    }
  
  return isUpdated;
}

/* This function updates a vector structure
    with the values contained in a vectPtr.
    Furthermore, it updates the init flag
    (TRUE) and the id value.    */
void updateVectStruct(vectStruct_t* vectStr, uint32_t id, uint32_t* vectPtr, bool_t* updated)
{
    if (updated != NULL)
    {
        *updated = FALSE;
        /* Check if pointers are valid or not. */
        if ((vectStr != NULL) && (vectPtr != NULL))
        {
            vectStr->id = id;
            uint32_t i;
            for (i = 0U; i < DIM; i++)
            {
                vectStr->vector[i] = *(vectPtr+i);
            }
            vectStr->init = TRUE;
            *updated = TRUE;
        }
    }
    else
    {
        nullBool = TRUE;
    }
}

/* This function checks if the info structure array is valid (i.e.
    each info structure has a not null vector structure pointer)
    and return an array of valid/invalid values. */
valid_t* checkValidInfoStruct(infoStruct_t infoStrArray[NB_OF_STR])
{
    valid_t* validArray = (valid_t*)malloc(sizeof(valid_t) * NB_OF_STR);
    uint32_t i;
    for (i = 0U; i < NB_OF_STR; i++)
    {
        if (infoStrArray[i].vectorStruct != NULL)
        {
            validArray[i] = VALID;
        }
        else
        {
            validArray[i] = INVALID;
        }
    }
    return validArray;
}

/* This function returns the number of elements in
    an array of vector structures equal to the given
    element. */
uint32_t countOccVectStructArray(vectStruct_t vectStrArray[NB_OF_STR], 
                                                 uint32_t elem)
{
    uint32_t nb_of_occ = 0U;
    uint32_t i;
    uint32_t j;
    for (i = 0U; i < NB_OF_STR; i++)
    {
        for (j = 0U; j < DIM; j++)
        {
            if (vectStrArray[i].vector[j] == elem)
            {
                nb_of_occ++;
            }
        }
    }
    return nb_of_occ;
}

/* This function checks if the given vector structure
    has at least one field different from zero and, in
    case, returns 1; otherwise, it returns 0. */
static uint32_t testVectStruct(vectStruct_t* vectStruct)
{
    uint32_t retVal = 0U;
    uint32_t i;
    uint32_t count = 0U;
    /* Check if pointer is valid or not. */
    if (vectStruct != NULL)
    {
        for (i = 0U; i < DIM; i++)
        {
            if (vectStruct->vector[i] != 0U)
            {
                count++;
            }
        }
        if (count != 0U)
        {
            retVal = 1U;
        }
    }
    return retVal;
}

/* This function checks if the vector structure is NULL. */
static uint32_t testNullStruct(vectStruct_t* vectStruct)
{
    uint32_t retVal;
    if (vectStruct == NULL)
    {
        retVal = 1U;
    }
    else
    {
        retVal = 0U;
    }
    return retVal;
}

/* This function checks the given info structure by using
    the given test function and returns TRUE if the test
    passes, otherwise FALSE. */
bool_t testInfoStruct(infoStruct_t* infoStruct, uint32_t (* funct)(vectStruct_t* vectStruct))
{
    bool_t success = FALSE;
    uint32_t retVal = 0U;
    /* Check if pointer is valid or not. */
    if ((infoStruct != NULL) && (funct != NULL))
    {
        /* If key == 0, the check fails */
        if (infoStruct->key != 0U)
        {
            retVal = (*funct)(infoStruct->vectorStruct);
            if (retVal == 1U)
            {
                success = TRUE;
            }
        }
    }
    return success;
}

/* This function fills a vector with the negated values of the given number. */
void fillNegVect(uint32_t* vect, uint32_t num)
{
    uint32_t dummy;
    uint32_t negNum;
    uint32_t i;
    
    /* Check if vector is valid or not. */
    if (vect != NULL)
    {
        negNum = negVal(num, &dummy);
        
        for (i = 0U; i < DIM; i++)
        {
            vect[i] = negNum;
        }
        
    }
    
}

static void copyVector(uint32_t* outVect, uint32_t* inVect, uint32_t size)
{
    uint32_t i;
    uint32_t tmpSize = size;
    
    if ((inVect != NULL) && (outVect != NULL))
    {
        if (size > DIM)
        {
            tmpSize = DIM;
        }
        
        for (i = 0U; i < tmpSize; i++)
        {
            outVect[i] = inVect[i];
        }
    }
}

uint32_t returnPtrVal(uint32_t* numPtr, uint32_t val)
{
    if (numPtr != NULL)
    {
        *numPtr = val;
    }
    return val;
}

/* This function */
void buildNegValVector(uint32_t* vectPtr)
{
    uint32_t i;
    uint32_t dummy;
    uint32_t tmpVal;    
    uint32_t simpleVect[DIM] = {1U, 2U, 3U, 4U, 5U};

    if (vectPtr != NULL)
    {
        copyVector(vectPtr, &simpleVect[0U], DIM);
        for (i = 0U; i < DIM; i++)
        {
            tmpVal = negVal(*(vectPtr + i), &dummy);
            vectPtr[i] = tmpVal;
        }
    }
}

/* This function prints a message */
bool_t printMsg(char* message)
{
    bool_t retSuccess = FALSE;
    if (message != NULL)
    {
        printf(message);
        retSuccess = TRUE;
    }
    else
    {
        printf("ERROR: MESSAGE NOT VALID\n");
    }
    return retSuccess;
}

/* This functions calculates and returns the product of sums
    of the elements of three vectors (sum[i] = a[i] + b[i] + c[i]).
    The number of sums is equal to min(size,DIM). */    
uint32_t prodSum(uint32_t* v1, uint32_t v2[], uint32_t v3[DIM], uint32_t size)
{
    uint32_t retVal = 1U;
    /* Check if vectors are valid or not. */
    if ((v1 != NULL) && (v2 != NULL) && (v3 != NULL))
    {
        uint32_t realSize = size;
        if (size > DIM)
        {
            realSize = DIM;
        }
        uint32_t sumVect[realSize];
        uint32_t i;
        for (i = 0U; i < realSize; i++)
        {
            sumVect[i] = v1[i] + v2[i] + v3[i];
            retVal = retVal * sumVect[i];
        }
    }
    else
    {
        retVal = 0U;
    }
    return retVal;
}

bool_t isNegativeNumber(int32_t num)
{
  bool_t isNegative = FALSE;
  if (num < 0)
    {
      isNegative = TRUE;
    }
  return isNegative;
}

bool_t isWrittenToFile(int32_t fd, void* buff, uint32_t size)
{
  bool_t isWritten = FALSE;
  if (buff != NULL)
    {
      write(fd, buff, size);
      isWritten = TRUE;
    }
  return isWritten;
}

static void updateValue(uint32_t* value, uint32_t newVal)
{
  if (value != NULL)
    {
      *value = newVal;
    }
}

void updateVectorValues(uint32_t* vect, uint32_t val, uint32_t size)
{
  uint32_t i;

  if (vect != NULL)
    {
      for (i = 0U; i < size; i++)
	{
	  updateValue(&vect[i], i * val);
	}
    }
}

uint32_t returnGlobalPtrValue()
{
  uint32_t retVal;
  retVal = *globalPtr;
  return retVal;
}

static void printErrorMsg(char* msg, uint32_t size)
{
  write(stdout, msg, size);
  //printf("Printed Error Msg\n");
  //fflush(stdout);
}

void call_exit(uint32_t code)
{
  exit(code);
  //return;
}

void printError(void)
{
  printErrorMsg("ERROR",64);
}

uint32_t check_exit(uint32_t* ptr)
{
  struct timeval tv;
  int retVal = gettimeofday(&tv, NULL);
  char buffMsg[64U] = "[ ";
  char timeMsg[64U];
  snprintf(&timeMsg[0U], sizeof(timeMsg), "%d", tv.tv_sec);
  strcat(&buffMsg[0U], &timeMsg[0U]);
  strcat(&buffMsg[0U], " , ");
  snprintf(&timeMsg[0U], sizeof(timeMsg), "%d", tv.tv_sec);
  strcat(&buffMsg[0U], " ] ");
  strcat(&buffMsg[0U], "##ERROR: found a null ptr!!!##\n");
  if (ptr == NULL)
    {
      printErrorMsg(&buffMsg[0U], 64U);
      call_exit(0U);
    }
  return 1U;
}

uint32_t checkPtrIntegrity(uint32_t* ptr)
{
  uint32_t retVal = 1U;
  CHECK_EXIT(ptr);
  *ptr = 17U;
  return retVal;
}

uint32_t retNegFormat(format_t format)
{
  return ~(uint32_t)format;
}

format_t getGlobalFormat()
{
  return globalFormat;
}

void setGlobalCode(code_t code)
{
  globalCode = code;
}

code_t getGlobalCode()
{
  return globalCode;
}

void retDefInfoStruct(infoStruct_t** ptr)
{
  if (ptr!= NULL)
    {
      *ptr = &defInfoStruct;
    }
}

void retInfoStructEntry(infoStruct_t** ptr, uint32_t index)
{
  bool_t ok = TRUE;
  if (ptr == NULL)
    {
      ok = FALSE;
    }
  if (index >= DIM)
    {
      ok = FALSE;
    }
  if (ok == TRUE)
    {
      *ptr = &infoStructArray[index];
    }
}

bool_t setFirstInfoStructEntry(infoStruct_t** ptr)
{
  bool_t success = TRUE;
  if (ptr != NULL)
    {
      if (*ptr != NULL)
	{
	  infoStructArray[0U].key = (*ptr)->key;
	  infoStructArray[0U].vectorStruct->id = ((*ptr)->vectorStruct)->id;
	}
      else
	{
	  success = FALSE;
	}
    }
  else
    {
      success = FALSE;
    }
  return success;
}

void initInfoStructEntry(infoStruct_t* infoStrPtr, uint32_t index)
{
  vectStruct_t tmpVectStr;
  if (index < DIM)
    {
      infoStrPtr = &infoStructArray[index];
      infoStrPtr->key = 0U;
      tmpVectStr = initVectStruct();
      infoStrPtr->vectorStruct = &tmpVectStr;
    }
}

bool_t validInfoStructPtr(infoStruct_t* infoStrPtr)
{
  bool_t valid = FALSE;
  if (infoStrPtr != NULL)
    {
      if (infoStrPtr->vectorStruct != NULL)
	{
	  valid = TRUE;
	}
    }
  return valid;
}

bool_t isValidDataStruct(dataStruct_t* data)
{
  bool_t isValid = TRUE;

  uint32_t i;

  if (data != NULL)
    {
      if (data->format == FMT_NULL)
	{
	  isValid = FALSE;
	}
      if (((data->id == NULL) || (data->id[0] = '\0')) && isValid)
	{
	  isValid = FALSE;
	}
      if ((data->buffStruct.valid == INVALID) && isValid)
	{
	  isValid = FALSE;
	}
      for (i = 0U; i < BUFF_SIZE && isValid; i++)
	{
	  if (data->buffStruct.buff[i] == 0xDEADU)
	    {
	      isValid = FALSE;
	    }
	}
    }
  else
    {
      printf("Warning: passed a null ptr!\n");
      isValid = FALSE;
    }
  return isValid;
}

void getEmptyDataStruct(dataStruct_t* data)
{
  char tmpId = '\0';

  uint32_t i;

  if (data != NULL)
    {
      data->format = FMT_NULL;
      memcpy(data->id, &tmpId, sizeof(tmpId));
      data->buffStruct.valid = INVALID;
      
      for (i = 0U; i < BUFF_SIZE; i++)
	{
	  data->buffStruct.buff[i] = 0xDEADU;
	}
    }
}

void getEmptySetStruct(setStruct_t* set)
{
  if (set != NULL)
    {
      set->flag = FALSE;

      if (set->data != NULL)
	{
	  getEmptyDataStruct(set->data);
	}
    }
}

void retGlobalSet(setStruct_t* set)
{
  if (set != NULL)
    {
      memcpy(set, &globalSet, sizeof(globalSet));
    }
}

void setGlobalSet(setStruct_t set)
{
  memcpy(&globalSet, &set, sizeof(set));
}

void fillOnesArray(uint32_t array[3])
{
  uint32_t i;
  for (i = 0U; i < 3U; i++)
    {
      array[i] = 1U;
    }
}

void printCharArray(char array[DIM])
{
  printf("Array: %s\n",array);
}

void setMatrix(uint32_t** ptr)
{
  if (ptr != NULL)
    {
      if (*ptr != NULL)
	{
	  memcpy(&matrix[0U], *ptr, sizeof(matrix));
	}
    }
}

void getFirstEntryMatrix(uint32_t* ptr)
{
  if (ptr != NULL)
    {
      memcpy(ptr, &matrix[0U], sizeof(matrix[0U]));
    }
}

void setGlobalBuffStruct(buffStruct_t buffStruct)
{
  memcpy(globalBuffStruct, &buffStruct, sizeof(buffStruct));
}

void getGlobalBuffStuct(buffStruct_t* buffStruct)
{
  if (buffStruct != NULL)
    {
      memcpy(buffStruct, globalBuffStruct, sizeof(*buffStruct));
    }
}

void getGlobalArraySet(setArrayStruct_t* ptr)
{
  if (ptr != NULL)
    {
      memcpy(ptr, &globalArraySet, sizeof(globalArraySet));
    }
}

void copyArray(uint32_t outArray[3U], uint32_t inArray[3U])
{
  uint32_t i;
  for (i = 0U; i < 3U; i++)
    {
      outArray[i] = inArray[i];
    }
}

bool_t isZeroArray(array_t array)
{
  bool_t ret = TRUE;
  uint32_t i;
  for (i = 0U; i < 2U && !ret; i++)
    {
      if (array[i] != 0U)
	{
	  ret = FALSE;
	}
    }
  return ret;
}

void copyCharArray(char* out, char* in, uint32_t size)
{
  uint32_t i;
  for (i = 0U; i < size; i++)
    {
      out[i] = in[i];
    }
}

void copyCharArrayTwice(char* out, char* in_1, char* in_2, uint32_t size_1, uint32_t size_2)
{
  copyCharArray(out, in_1, size_1);
  copyCharArray(&out[size_1], in_2, size_2);
}

void getGlobalPair(pair_t* pair)
{
  if (pair != NULL)
    {
      memcpy(pair, &globalPair, sizeof(globalPair));
    }
}

void setGlobalPair(pair_t* pair)
{
  if (pair != NULL)
    {
      memcpy(&globalPair, pair, sizeof(globalPair));
    }
}

void copyPair(pair_t pair)
{
  memcpy(&globalPair, &pair, sizeof(pair));
}

void copyEntry(entry_t entry)
{
  memcpy(&globalEntry, &entry, sizeof(entry));
}

uint32_t countCharOcc(char msg[64], char c)
{
  uint32_t counter = 0U;
  uint32_t i;
  for (i = 0U; i < 64U; i++)
    {
      if (msg[i] == c)
	{
	  counter++;
	}
    }
  return counter;
}

bool_t setDefDataStruct(dataInfo_t dataInfo)
{
  bool_t success = TRUE;
  memcpy(&globalData, &dataInfo, sizeof(dataInfo));
  return success;
}

dataInfo_t getDefDataStruct()
{
  return globalData;
}

static void logg(char* message)
{
  printErrorMsg(message, 256);
}

void errorLog(char *msg) 
{
  logg(msg);
}

bool_t assertCheck(void* ptr, char* msg, char* file, int line, uint32_t checkVal)
{
  bool_t res = TRUE;
  if (ptr == NULL)
    {
      errorLog(msg);
      res = FALSE;
    }
  return res;
}

bool_t checkPtrs(void* ptr_1, void* ptr_2, void* ptr_3, char* file, int line, uint32_t checkVal)
{
  bool_t res;
  bool_t res_1;
  bool_t res_2;
  bool_t res_3;
  char msg[256] = "#abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ#@$%~()*&^ FATAL ERROR!!!!!! \n\t\r\n\nFOUND A NULL POINTER!!! SYSTEM ENTERS HALT STATE!!!! ########--/ERROR nfgkl1234567890`!@#$%^()*-_=+|{}{}[];;;nvmhfahu###\n";
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  /* Call again assertCheck */
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  /* Once more */
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res = (res_1 && res_2 && res_3);
  return res;
}

bool_t checkDataStructId(void)
{
  bool_t retVal = TRUE;
  if (globalDataStruct.id == NULL)
    {
      retVal = FALSE;
    }
  return retVal;
}

void getGlobalSetPtr(setStruct_t** ptr)
{
  if (ptr != NULL)
    {
      *ptr = globalSetPtr;
    }
}

/* Main program */
int main()
{
    uint32_t num = 10U;
    uint32_t retNum;
    uint32_t negNum = negVal(num, &retNum);
    uint32_t v1[] = {1, 2, 3};
    uint32_t v2[] = {4, 5, 6};
    uint32_t v3[DIM] = {7, 8, 9, 10, 11};
    uint32_t size = 3U;
    uint32_t retProdSum = prodSum(&v1[0U], v2, v3, size);
    printf("ProdSum value: %d\n",retProdSum);
    vectStruct_t v = initVectStruct();
    uint32_t i;
        
    printf("INITIALIZATION\n");
    for (i = 0U; i < DIM; i++)
    {
        printf("v.vector[%d]: %d\n",i,v.vector[i]);
    }
    printf("UPDATE\n");
    bool_t updated = FALSE;
    updateVectStruct(&v, 255U, &v3[0U],&updated);
    for (i = 0U; i < DIM; i++)
    {
        printf("v.vector[%d]: %d\n",i,v.vector[i]);
    }
    printf("updated: %d\n",updated);
    infoStruct_t infoStr;
    infoStr.key = 1U;
    infoStr.vectorStruct = &v;
    bool_t success = testInfoStruct(&infoStr, testVectStruct);
    printf("Info structure test result: %d\n",success);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "interface.h"
#include "types.h"
#include "utils.h"

/* MACRO DEFINITION */

#define DIM 5U
#define NB_OF_STR 3
#define BUFF_SIZE 64U

#define CHECK_EXIT(ptr) \
  do {		        \
    check_exit(ptr);    \
  } while (0)

/* END MACRO */

/* TYPEDEFS */
typedef uint32_t array_t[2];

array_t dummy_dummy;

typedef enum
  {
    FMT_NULL,
    FMT_DEC,
    FMT_CHAR
  } format_t;

typedef enum
  {
    CODE_0=255,
    CODE_1=1,
    CODE_2
  } code_t;

struct vectStruct_t
{
    uint32_t id;
    uint32_t vector[DIM];
    bool_t init;
};

typedef struct vectStruct_t vectStruct_t;

struct infoStruct_t
{
    uint32_t key;
    vectStruct_t* vectorStruct;
};

typedef struct infoStruct_t infoStruct_t;

typedef enum
{
    VALID,
    INVALID
} valid_t;

struct buffStruct_t
{
  valid_t valid;
  uint32_t buff[BUFF_SIZE];
};

typedef struct buffStruct_t buffStruct_t;

struct dataStruct_t
{
  format_t format;
  char* id;
  buffStruct_t buffStruct;
};

typedef struct dataStruct_t dataStruct_t;

struct setStruct_t
{
  bool_t flag;
  dataStruct_t* data;
};

typedef struct setStruct_t setStruct_t;

struct setArrayStruct_t
{
  uint32_t setArraySize;
  setStruct_t setArray[DIM];
};

typedef struct setArrayStruct_t setArrayStruct_t;

struct dataInfo_t
{
  dataStruct_t data[DIM];
  uint32_t nb_of_data;
  uint32_t current;
};

typedef struct dataInfo_t dataInfo_t;
/* END TYPEDEFS */

/* STATIC VARIABLES */
static bool_t nullBool = FALSE;
static uint32_t globalVal;
static uint32_t* globalPtr = NULL;
infoStruct_t infoStructArray[DIM];
infoStruct_t defInfoStruct;
static format_t globalFormat = FMT_NULL;
static setStruct_t globalSet;
code_t globalCode;
uint32_t matrix[DIM][DIM];
static buffStruct_t* globalBuffStruct;
static setArrayStruct_t globalArraySet;
static pair_t globalPair;
static entry_t globalEntry;
static dataStruct_t globalDataStruct;
static dataInfo_t globalData;
static setStruct_t* globalSetPtr;
/*END STATIC VARIABLES */

/* This function initializes a vector: it sets the value of vect[i] as i + 1, with i=1...DIM */
void initVector(uint32_t* vectPtr)
{
    uint32_t i;
    if (vectPtr != NULL)
    {
        for (i = 0U; i < DIM; i++)
        {
            vectPtr[i] = INCR_VAL(i);
        }
    }
}

void segFaultVector(uint32_t* vectPtr)
{
    uint32_t i;
    vectPtr = 0;
    i = *vectPtr;
}
/* This function returns a vector structure.
    Its fields are filled with default values. */
vectStruct_t initVectStruct()
{
    vectStruct_t retStr;
    uint32_t i;
    retStr.id = 0U;
    retStr.init = FALSE;
    for (i = 0U; i < DIM; i++)
    {
        retStr.vector[i] = 0U;
    }
    return retStr;
}

bool_t checkVectStruct(vectStruct_t vectStr)
{
  bool_t isUpdated = FALSE;

  if (vectStr.init == TRUE)
    {
      isUpdated = TRUE;
      printf("VectStr has already been updated... id = %d\n", vectStr.id);
    }
  else
    {
      printf("VectStr not updated\n");
    }
  
  return isUpdated;
}

/* This function updates a vector structure
    with the values contained in a vectPtr.
    Furthermore, it updates the init flag
    (TRUE) and the id value.    */
void updateVectStruct(vectStruct_t* vectStr, uint32_t id, uint32_t* vectPtr, bool_t* updated)
{
    if (updated != NULL)
    {
        *updated = FALSE;
        /* Check if pointers are valid or not. */
        if ((vectStr != NULL) && (vectPtr != NULL))
        {
            vectStr->id = id;
            uint32_t i;
            for (i = 0U; i < DIM; i++)
            {
                vectStr->vector[i] = *(vectPtr+i);
            }
            vectStr->init = TRUE;
            *updated = TRUE;
        }
    }
    else
    {
        nullBool = TRUE;
    }
}

/* This function checks if the info structure array is valid (i.e.
    each info structure has a not null vector structure pointer)
    and return an array of valid/invalid values. */
valid_t* checkValidInfoStruct(infoStruct_t infoStrArray[NB_OF_STR])
{
    valid_t* validArray = (valid_t*)malloc(sizeof(valid_t) * NB_OF_STR);
    uint32_t i;
    for (i = 0U; i < NB_OF_STR; i++)
    {
        if (infoStrArray[i].vectorStruct != NULL)
        {
            validArray[i] = VALID;
        }
        else
        {
            validArray[i] = INVALID;
        }
    }
    return validArray;
}

/* This function returns the number of elements in
    an array of vector structures equal to the given
    element. */
uint32_t countOccVectStructArray(vectStruct_t vectStrArray[NB_OF_STR], 
                                                 uint32_t elem)
{
    uint32_t nb_of_occ = 0U;
    uint32_t i;
    uint32_t j;
    for (i = 0U; i < NB_OF_STR; i++)
    {
        for (j = 0U; j < DIM; j++)
        {
            if (vectStrArray[i].vector[j] == elem)
            {
                nb_of_occ++;
            }
        }
    }
    return nb_of_occ;
}

/* This function checks if the given vector structure
    has at least one field different from zero and, in
    case, returns 1; otherwise, it returns 0. */
static uint32_t testVectStruct(vectStruct_t* vectStruct)
{
    uint32_t retVal = 0U;
    uint32_t i;
    uint32_t count = 0U;
    /* Check if pointer is valid or not. */
    if (vectStruct != NULL)
    {
        for (i = 0U; i < DIM; i++)
        {
            if (vectStruct->vector[i] != 0U)
            {
                count++;
            }
        }
        if (count != 0U)
        {
            retVal = 1U;
        }
    }
    return retVal;
}

/* This function checks if the vector structure is NULL. */
static uint32_t testNullStruct(vectStruct_t* vectStruct)
{
    uint32_t retVal;
    if (vectStruct == NULL)
    {
        retVal = 1U;
    }
    else
    {
        retVal = 0U;
    }
    return retVal;
}

/* This function checks the given info structure by using
    the given test function and returns TRUE if the test
    passes, otherwise FALSE. */
bool_t testInfoStruct(infoStruct_t* infoStruct, uint32_t (* funct)(vectStruct_t* vectStruct))
{
    bool_t success = FALSE;
    uint32_t retVal = 0U;
    /* Check if pointer is valid or not. */
    if ((infoStruct != NULL) && (funct != NULL))
    {
        /* If key == 0, the check fails */
        if (infoStruct->key != 0U)
        {
            retVal = (*funct)(infoStruct->vectorStruct);
            if (retVal == 1U)
            {
                success = TRUE;
            }
        }
    }
    return success;
}

/* This function fills a vector with the negated values of the given number. */
void fillNegVect(uint32_t* vect, uint32_t num)
{
    uint32_t dummy;
    uint32_t negNum;
    uint32_t i;
    
    /* Check if vector is valid or not. */
    if (vect != NULL)
    {
        negNum = negVal(num, &dummy);
        
        for (i = 0U; i < DIM; i++)
        {
            vect[i] = negNum;
        }
        
    }
    
}

static void copyVector(uint32_t* outVect, uint32_t* inVect, uint32_t size)
{
    uint32_t i;
    uint32_t tmpSize = size;
    
    if ((inVect != NULL) && (outVect != NULL))
    {
        if (size > DIM)
        {
            tmpSize = DIM;
        }
        
        for (i = 0U; i < tmpSize; i++)
        {
            outVect[i] = inVect[i];
        }
    }
}

uint32_t returnPtrVal(uint32_t* numPtr, uint32_t val)
{
    if (numPtr != NULL)
    {
        *numPtr = val;
    }
    return val;
}

/* This function */
void buildNegValVector(uint32_t* vectPtr)
{
    uint32_t i;
    uint32_t dummy;
    uint32_t tmpVal;    
    uint32_t simpleVect[DIM] = {1U, 2U, 3U, 4U, 5U};

    if (vectPtr != NULL)
    {
        copyVector(vectPtr, &simpleVect[0U], DIM);
        for (i = 0U; i < DIM; i++)
        {
            tmpVal = negVal(*(vectPtr + i), &dummy);
            vectPtr[i] = tmpVal;
        }
    }
}

/* This function prints a message */
bool_t printMsg(char* message)
{
    bool_t retSuccess = FALSE;
    if (message != NULL)
    {
        printf(message);
        retSuccess = TRUE;
    }
    else
    {
        printf("ERROR: MESSAGE NOT VALID\n");
    }
    return retSuccess;
}

/* This functions calculates and returns the product of sums
    of the elements of three vectors (sum[i] = a[i] + b[i] + c[i]).
    The number of sums is equal to min(size,DIM). */    
uint32_t prodSum(uint32_t* v1, uint32_t v2[], uint32_t v3[DIM], uint32_t size)
{
    uint32_t retVal = 1U;
    /* Check if vectors are valid or not. */
    if ((v1 != NULL) && (v2 != NULL) && (v3 != NULL))
    {
        uint32_t realSize = size;
        if (size > DIM)
        {
            realSize = DIM;
        }
        uint32_t sumVect[realSize];
        uint32_t i;
        for (i = 0U; i < realSize; i++)
        {
            sumVect[i] = v1[i] + v2[i] + v3[i];
            retVal = retVal * sumVect[i];
        }
    }
    else
    {
        retVal = 0U;
    }
    return retVal;
}

bool_t isNegativeNumber(int32_t num)
{
  bool_t isNegative = FALSE;
  if (num < 0)
    {
      isNegative = TRUE;
    }
  return isNegative;
}

bool_t isWrittenToFile(int32_t fd, void* buff, uint32_t size)
{
  bool_t isWritten = FALSE;
  if (buff != NULL)
    {
      write(fd, buff, size);
      isWritten = TRUE;
    }
  return isWritten;
}

static void updateValue(uint32_t* value, uint32_t newVal)
{
  if (value != NULL)
    {
      *value = newVal;
    }
}

void updateVectorValues(uint32_t* vect, uint32_t val, uint32_t size)
{
  uint32_t i;

  if (vect != NULL)
    {
      for (i = 0U; i < size; i++)
	{
	  updateValue(&vect[i], i * val);
	}
    }
}

uint32_t returnGlobalPtrValue()
{
  uint32_t retVal;
  retVal = *globalPtr;
  return retVal;
}

static void printErrorMsg(char* msg, uint32_t size)
{
  write(stdout, msg, size);
  //printf("Printed Error Msg\n");
  //fflush(stdout);
}

void call_exit(uint32_t code)
{
  exit(code);
  //return;
}

void printError(void)
{
  printErrorMsg("ERROR",64);
}

uint32_t check_exit(uint32_t* ptr)
{
  struct timeval tv;
  int retVal = gettimeofday(&tv, NULL);
  char buffMsg[64U] = "[ ";
  char timeMsg[64U];
  snprintf(&timeMsg[0U], sizeof(timeMsg), "%d", tv.tv_sec);
  strcat(&buffMsg[0U], &timeMsg[0U]);
  strcat(&buffMsg[0U], " , ");
  snprintf(&timeMsg[0U], sizeof(timeMsg), "%d", tv.tv_sec);
  strcat(&buffMsg[0U], " ] ");
  strcat(&buffMsg[0U], "##ERROR: found a null ptr!!!##\n");
  if (ptr == NULL)
    {
      printErrorMsg(&buffMsg[0U], 64U);
      call_exit(0U);
    }
  return 1U;
}

uint32_t checkPtrIntegrity(uint32_t* ptr)
{
  uint32_t retVal = 1U;
  CHECK_EXIT(ptr);
  *ptr = 17U;
  return retVal;
}

uint32_t retNegFormat(format_t format)
{
  return ~(uint32_t)format;
}

format_t getGlobalFormat()
{
  return globalFormat;
}

void setGlobalCode(code_t code)
{
  globalCode = code;
}

code_t getGlobalCode()
{
  return globalCode;
}

void retDefInfoStruct(infoStruct_t** ptr)
{
  if (ptr!= NULL)
    {
      *ptr = &defInfoStruct;
    }
}

void retInfoStructEntry(infoStruct_t** ptr, uint32_t index)
{
  bool_t ok = TRUE;
  if (ptr == NULL)
    {
      ok = FALSE;
    }
  if (index >= DIM)
    {
      ok = FALSE;
    }
  if (ok == TRUE)
    {
      *ptr = &infoStructArray[index];
    }
}

bool_t setFirstInfoStructEntry(infoStruct_t** ptr)
{
  bool_t success = TRUE;
  if (ptr != NULL)
    {
      if (*ptr != NULL)
	{
	  infoStructArray[0U].key = (*ptr)->key;
	  infoStructArray[0U].vectorStruct->id = ((*ptr)->vectorStruct)->id;
	}
      else
	{
	  success = FALSE;
	}
    }
  else
    {
      success = FALSE;
    }
  return success;
}

void initInfoStructEntry(infoStruct_t* infoStrPtr, uint32_t index)
{
  vectStruct_t tmpVectStr;
  if (index < DIM)
    {
      infoStrPtr = &infoStructArray[index];
      infoStrPtr->key = 0U;
      tmpVectStr = initVectStruct();
      infoStrPtr->vectorStruct = &tmpVectStr;
    }
}

bool_t validInfoStructPtr(infoStruct_t* infoStrPtr)
{
  bool_t valid = FALSE;
  if (infoStrPtr != NULL)
    {
      if (infoStrPtr->vectorStruct != NULL)
	{
	  valid = TRUE;
	}
    }
  return valid;
}

bool_t isValidDataStruct(dataStruct_t* data)
{
  bool_t isValid = TRUE;

  uint32_t i;

  if (data != NULL)
    {
      if (data->format == FMT_NULL)
	{
	  isValid = FALSE;
	}
      if (((data->id == NULL) || (data->id[0] = '\0')) && isValid)
	{
	  isValid = FALSE;
	}
      if ((data->buffStruct.valid == INVALID) && isValid)
	{
	  isValid = FALSE;
	}
      for (i = 0U; i < BUFF_SIZE && isValid; i++)
	{
	  if (data->buffStruct.buff[i] == 0xDEADU)
	    {
	      isValid = FALSE;
	    }
	}
    }
  else
    {
      printf("Warning: passed a null ptr!\n");
      isValid = FALSE;
    }
  return isValid;
}

void getEmptyDataStruct(dataStruct_t* data)
{
  char tmpId = '\0';

  uint32_t i;

  if (data != NULL)
    {
      data->format = FMT_NULL;
      memcpy(data->id, &tmpId, sizeof(tmpId));
      data->buffStruct.valid = INVALID;
      
      for (i = 0U; i < BUFF_SIZE; i++)
	{
	  data->buffStruct.buff[i] = 0xDEADU;
	}
    }
}

void getEmptySetStruct(setStruct_t* set)
{
  if (set != NULL)
    {
      set->flag = FALSE;

      if (set->data != NULL)
	{
	  getEmptyDataStruct(set->data);
	}
    }
}

void retGlobalSet(setStruct_t* set)
{
  if (set != NULL)
    {
      memcpy(set, &globalSet, sizeof(globalSet));
    }
}

void setGlobalSet(setStruct_t set)
{
  memcpy(&globalSet, &set, sizeof(set));
}

void fillOnesArray(uint32_t array[3])
{
  uint32_t i;
  for (i = 0U; i < 3U; i++)
    {
      array[i] = 1U;
    }
}

void printCharArray(char array[DIM])
{
  printf("Array: %s\n",array);
}

void setMatrix(uint32_t** ptr)
{
  if (ptr != NULL)
    {
      if (*ptr != NULL)
	{
	  memcpy(&matrix[0U], *ptr, sizeof(matrix));
	}
    }
}

void getFirstEntryMatrix(uint32_t* ptr)
{
  if (ptr != NULL)
    {
      memcpy(ptr, &matrix[0U], sizeof(matrix[0U]));
    }
}

void setGlobalBuffStruct(buffStruct_t buffStruct)
{
  memcpy(globalBuffStruct, &buffStruct, sizeof(buffStruct));
}

void getGlobalBuffStuct(buffStruct_t* buffStruct)
{
  if (buffStruct != NULL)
    {
      memcpy(buffStruct, globalBuffStruct, sizeof(*buffStruct));
    }
}

void getGlobalArraySet(setArrayStruct_t* ptr)
{
  if (ptr != NULL)
    {
      memcpy(ptr, &globalArraySet, sizeof(globalArraySet));
    }
}

void copyArray(uint32_t outArray[3U], uint32_t inArray[3U])
{
  uint32_t i;
  for (i = 0U; i < 3U; i++)
    {
      outArray[i] = inArray[i];
    }
}

bool_t isZeroArray(array_t array)
{
  bool_t ret = TRUE;
  uint32_t i;
  for (i = 0U; i < 2U && !ret; i++)
    {
      if (array[i] != 0U)
	{
	  ret = FALSE;
	}
    }
  return ret;
}

void copyCharArray(char* out, char* in, uint32_t size)
{
  uint32_t i;
  for (i = 0U; i < size; i++)
    {
      out[i] = in[i];
    }
}

void copyCharArrayTwice(char* out, char* in_1, char* in_2, uint32_t size_1, uint32_t size_2)
{
  copyCharArray(out, in_1, size_1);
  copyCharArray(&out[size_1], in_2, size_2);
}

void getGlobalPair(pair_t* pair)
{
  if (pair != NULL)
    {
      memcpy(pair, &globalPair, sizeof(globalPair));
    }
}

void setGlobalPair(pair_t* pair)
{
  if (pair != NULL)
    {
      memcpy(&globalPair, pair, sizeof(globalPair));
    }
}

void copyPair(pair_t pair)
{
  memcpy(&globalPair, &pair, sizeof(pair));
}

void copyEntry(entry_t entry)
{
  memcpy(&globalEntry, &entry, sizeof(entry));
}

uint32_t countCharOcc(char msg[64], char c)
{
  uint32_t counter = 0U;
  uint32_t i;
  for (i = 0U; i < 64U; i++)
    {
      if (msg[i] == c)
	{
	  counter++;
	}
    }
  return counter;
}

bool_t setDefDataStruct(dataInfo_t dataInfo)
{
  bool_t success = TRUE;
  memcpy(&globalData, &dataInfo, sizeof(dataInfo));
  return success;
}

dataInfo_t getDefDataStruct()
{
  return globalData;
}

static void logg(char* message)
{
  printErrorMsg(message, 256);
}

void errorLog(char *msg) 
{
  logg(msg);
}

bool_t assertCheck(void* ptr, char* msg, char* file, int line, uint32_t checkVal)
{
  bool_t res = TRUE;
  if (ptr == NULL)
    {
      errorLog(msg);
      res = FALSE;
    }
  return res;
}

bool_t checkPtrs(void* ptr_1, void* ptr_2, void* ptr_3, char* file, int line, uint32_t checkVal)
{
  bool_t res;
  bool_t res_1;
  bool_t res_2;
  bool_t res_3;
  char msg[256] = "#abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ#@$%~()*&^ FATAL ERROR!!!!!! \n\t\r\n\nFOUND A NULL POINTER!!! SYSTEM ENTERS HALT STATE!!!! ########--/ERROR nfgkl1234567890`!@#$%^()*-_=+|{}{}[];;;nvmhfahu###\n";
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  /* Call again assertCheck */
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  /* Once more */
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res_1 = assertCheck(ptr_1, &msg[0U], file, line, checkVal);
  res_2 = assertCheck(ptr_2, &msg[0U], file, line, checkVal);
  res_3 = assertCheck(ptr_3, &msg[0U], file, line, checkVal);
  res = (res_1 && res_2 && res_3);
  return res;
}

bool_t checkDataStructId(void)
{
  bool_t retVal = TRUE;
  if (globalDataStruct.id == NULL)
    {
      retVal = FALSE;
    }
  return retVal;
}

void getGlobalSetPtr(setStruct_t** ptr)
{
  if (ptr != NULL)
    {
      *ptr = globalSetPtr;
    }
}

/* Main program */
int main()
{
    uint32_t num = 10U;
    uint32_t retNum;
    uint32_t negNum = negVal(num, &retNum);
    uint32_t v1[] = {1, 2, 3};
    uint32_t v2[] = {4, 5, 6};
    uint32_t v3[DIM] = {7, 8, 9, 10, 11};
    uint32_t size = 3U;
    uint32_t retProdSum = prodSum(&v1[0U], v2, v3, size);
    printf("ProdSum value: %d\n",retProdSum);
    vectStruct_t v = initVectStruct();
    uint32_t i;
        
    printf("INITIALIZATION\n");
    for (i = 0U; i < DIM; i++)
    {
        printf("v.vector[%d]: %d\n",i,v.vector[i]);
    }
    printf("UPDATE\n");
    bool_t updated = FALSE;
    updateVectStruct(&v, 255U, &v3[0U],&updated);
    for (i = 0U; i < DIM; i++)
    {
        printf("v.vector[%d]: %d\n",i,v.vector[i]);
    }
    printf("updated: %d\n",updated);
    infoStruct_t infoStr;
    infoStr.key = 1U;
    infoStr.vectorStruct = &v;
    bool_t success = testInfoStruct(&infoStr, testVectStruct);
    printf("Info structure test result: %d\n",success);
    return 0;
}

