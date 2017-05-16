// constants
#ifndef A3SEARCH_CONSTANTS_H
#define A3SEARCH_CONSTANTS_H

#define MAXIMUM_WORD_LENGTH 40 
#define STOPWORDS_FILE_NAME "stopwords.txt"
#define INDEX_SUFFIX ".idx"
#define DICTIONARY_FILE_NAME "dict"
#define POINTER_DOC_FREQ_FILE_NAME "pointer_doc_freq"
#define POSTINGS_FILE_NAME "postings"
#define FREQUENCY_FILE_NAME "frequency"

#define FLUSH_SIZE 32      // in megabytes, this is a maximum, actual size less
#define POSTING_EL_SIZE 16 // in bytes
#define FREQ_EL_SIZE    16 // in bytes
#define POSTINGS_BEFORE_FLUSH \
(FLUSH_SIZE * 1024 * 1024 / (POSTING_EL_SIZE + FREQ_EL_SIZE))

#endif
