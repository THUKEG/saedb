Readme of LDA Toolkit

1.Build
We recommend to build the project outside the source tree. To do this, please go to the root of the SAE project and execute the following commands:

mkdir build
cd build
cmake ..
make

2.Run toolkit
Put the "input" folder under the path below:
YOUR_SAE_PATH/build/toolkit/LDA/

Still under this path, execute:

./lda_test

Then the toolkit will run with the default parameter and test input. The output will save in:
YOUR_SAE_PATH/build/toolkit/LDA/output/

To set parameter, execute:
./lda_test -h 
for help.

3.Write a separater file
To know how to write a separater file to guide the toolkit read the input documents, please see the guidance in our separater file at:
YOUR_SAE_PATH/build/toolkit/LDA/input/input_document/sep.txt

4.Input and Output of this toolkit
Input: 
1.A document file with each line containing a document;
2.A separater file to guide document ingestion.
Output: 
1.The topic model with the format:
  Doc0's topic distribution line
  Doc1's topic distribution line
  ....
2.The word distribution, where the nth line shows the heated words of the nth topic.
3.For every topic k, show the documents in which topic k has the largest probability in their topic distribution. 
4.The document title list.

For any question, you can directly contact me at v-qicche@hotmail.com.





