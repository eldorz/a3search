# a3search
## comp9319 assignment 3 keyword and concept search  
### Laughlin Dawes z3106483 May 2017  

Simple reverse index implementation, with storage of the frequency of each
term in each document.

To build the index, each document in the corpus is read sequentially. Each word
is ignored if in a stopword list.  It is then stemmed using the Oleander
stemming library, and added to an ordered map (likely to be a red/black tree)
of tokens. For each token, a postings list and a list of word frequencies for
those postings is maintained.  For example, if there are three files, the first
containing the word 'banana' 10 times, the second 4 times, and the third not at
all, the postings list for banana would be {0,1} and the frequency list {10, 4}.

When a certain number of postings have been made, it can be predicted that the
memory limit will be being approached.  At this point the map is written to
four files - the dictionary, which is a simple text file with one word per line;
the postings file, which is just a serialised sequence of integers; the
frequency file, also a serialised sequence of integers; and finally the pointer
file, which is a sequence of (pointer, doc_freq) pairs, where pointer is the
position in the postings and frequency files, and doc_freq is the number of
documents where that word exists. The data in RAM is then cleared and
document reading commences as before.

Once completed, this process potentially leaves many sets of these 4 files in
the index folder.  These are merged into one large file using a serial merge
technique.  The first two files are merged to a temporary file, that temporary
file is merged with the third file to a second temporary file, which is merged
with the fourth file to a third temporary file, and so on.  Files are deleted
as the process progresses to avoid excessive disc space use.

The completed index can then be queried efficiently.  The entire dictionary is
loaded into RAM as an unordered_map (a hash table), where the key is a word and
the value is the equivalent pair of values from the (pointer, doc_freq) file.
This gives us constant-time lookup and retrieval from the posting and frequency 
lists.

Finally, to perform concept search, each search term is passed to the Wordnet
library to obtain a set of synonyms.  Given the power of Wordnet, a broader
definition of word concept could easily be applied, such as matching hypernyms
(coordinate words). For the purpose of this project however, simple synonyms 
were considered appropriate. Each synonym is then searched in the index, and
obtained frequencies multiplied by the scaling factor.  Only files that match
(either by exact match or concept) all of the search terms are returned, 
sorted by filename then reverse stable sorted by scaled term frequency.


### Oleander stemming library:
Copyright (c) 2016, Oleander Software, Ltd.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of Oleander Software nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

### WordNet 3.0 license:
WordNet Release 3.0 This software and database is being provided to you, the LICENSEE, by Princeton University under the following license. By obtaining, using and/or copying this software and database, you agree that you have read, understood, and will comply with these terms and conditions.: Permission to use, copy, modify and distribute this software and database and its documentation for any purpose and without fee or royalty is hereby granted, provided that you agree to comply with the following copyright notice and statements, including the disclaimer, and that the same appear on ALL copies of the software, database and documentation, including modifications that you make for internal use or for distribution. WordNet 3.0 Copyright 2006 by Princeton University. All rights reserved. THIS SOFTWARE AND DATABASE IS PROVIDED "AS IS" AND PRINCETON UNIVERSITY MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED. BY WAY OF EXAMPLE, BUT NOT LIMITATION, PRINCETON UNIVERSITY MAKES NO REPRESENTATIONS OR WARRANTIES OF MERCHANT- ABILITY OR FITNESS FOR ANY PARTICULAR PURPOSE OR THAT THE USE OF THE LICENSED SOFTWARE, DATABASE OR DOCUMENTATION WILL NOT INFRINGE ANY THIRD PARTY PATENTS, COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS. The name of Princeton University or Princeton may not be used in advertising or publicity pertaining to distribution of the software and/or database. Title to copyright in this software, database and any associated documentation shall at all times remain with Princeton University and LICENSEE agrees to preserve same.
