#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iomanip>
using namespace std;

#define PRINT_COUNT 0
#define PRINT_RESULT 1

#include "opencv2/core/utility.hpp"
#include "opencv2/core/ocl.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;

class TestFile
{
private:
    char * pch;

    ifstream _inputFile;
    ofstream _outputFile;
    ofstream _resultFile;

    vector<string> imgBuffer;
    vector<string> testBuffer;
    vector<string> outputBuffer;
    vector<string> resultBuffer;

public:
    //初始化文件檔
    void InitTestFile(const char* testPath)
    {
        char inputFileName[100];
        char outputFileName[100];
        char resultFileName[100];

        strcpy(inputFileName, testPath);
        strcat(inputFileName, "inputTest.txt");

        strcpy(outputFileName, testPath);
        strcat(outputFileName, "outputTest.txt");

        strcpy(resultFileName, testPath);
        strcat(resultFileName, "testResult.txt");

        _inputFile.open(inputFileName, std::ifstream::in);
        _outputFile.open(outputFileName , ofstream::out | std::ios::trunc);
        _resultFile.open(resultFileName, ofstream::out | std::ios::trunc);
        InitTestData();
    }

    //初始化檔案資料＆讀取input檔
    void InitTestData()
    {
        const char *delim = ",";
        string buffer;
        string img;
        string test;
        char *charBuffer;
        while(_inputFile >> buffer)
        {
            //string to char*
            charBuffer = new char[buffer.length() + 1];
            strcpy(charBuffer, buffer.c_str());

            //strtok and push to vector
            pch = strtok(charBuffer, delim);
            img = string(pch); //char* to string
            imgBuffer.push_back(img);
            //cout << "push " << img << " to img vector" << endl;

            //strtok and push to vector
            pch = strtok(NULL, delim);
            test = string(pch); //char* to string
            testBuffer.push_back(test);
            //cout << "push " << test << " to test vector" << endl;

            outputBuffer.resize(imgBuffer.size());
            resultBuffer.resize(imgBuffer.size());
        }
    }

    //取得指定index圖片名稱
    string GetImgByIndex(int index)
    {
        return imgBuffer[index];
    }

    //顯示vector內容//debug用
    void ViewVector(char c)
    {
        switch (c)
        {
            case 'i':
            {
                cout << "-list image name vector-" << endl;
                for(int i  = 0; i < imgBuffer.size(); i++)
                {
                    cout << "image vector " << i << " = " << imgBuffer[i] << endl;
                }
                break;
            }
            case 't':
            {
                cout << "-list test vector-" << endl;
                for(int i  = 0; i < testBuffer.size(); i++)
                {
                    cout << "test vector " << i << " = " << testBuffer[i] << endl;
                }
                break;
            }
            case 'o':
            {
                cout << "-list output vector-" << endl;
                 for(int i  = 0; i < outputBuffer.size(); i++)
                {
                    if(outputBuffer[i].size() < 1)continue;
                    cout << "output vector " << i << " = " << outputBuffer[i] << endl;
                }
                break;
            }
            case 'r':
            {
                cout << "-list result vector-" << endl;
                 for(int i  = 0; i < resultBuffer.size(); i++)
                {
                     if(outputBuffer[i].size() < 1)continue;
                    cout << "result vector " << i << " = " << resultBuffer[i] << endl;
                }
                break;
            }
            default:
                cout << "default" << endl;
                break;
        }
    }

    //把資料寫進output vector中
    void WriteToOutput(string str)
    {
        outputBuffer.push_back(str);
    }

     //把資料寫進output vector指定Index中
    void WriteToOutputByIndex(string str, int index)
    {
        outputBuffer[index] = str;
    }

    //把資料寫進文件檔中
    void WriteDownOutput()
    {
        for(int index = 0; index < outputBuffer.size(); index++)
        {
            if(outputBuffer[index].size() < 1)continue;
            _outputFile << outputBuffer[index] << endl;
        }
    }

    //測試結果output
    void MatchResult()
    {
        ViewVector('o');
        //ViewVector('t');
        char outputCharArr[100];
        char testCharArr[100];
        for(int index = 0; index < outputBuffer.size(); index++)
         {
            //String To Char[] 轉型
            strcpy(outputCharArr, outputBuffer[index].c_str());
            strcpy(testCharArr, testBuffer[index].c_str());

            char ignoreChar[10] = "ignore";
            int ignoreResultTest = MatchNChar(outputCharArr, ignoreChar, 6);
            if(ignoreResultTest == 1)
            {
                resultBuffer[index] = "ignore";
                continue;
            }

            if(outputBuffer[index].size() < 1)
            {
                //resultBuffer.push_back("none");
                continue;
            }

            //比對測資與output是否一樣
            int result = MatchChar(outputCharArr, testCharArr);

            //將結果丟進resultBuffer中
            if(result == 1)resultBuffer[index] = "true";
            if(result == -1)resultBuffer[index] = "false";
        }
        //ViewVector('r');

        //所有測試結果寫進文件中
        for(int index = 0; index < resultBuffer.size(); index++)
        {
            if(resultBuffer[index].size() < 1)continue;
            _resultFile << resultBuffer[index] << endl;
        }
    }

    //列出成功的test
    int ListSuccessTest(int mode)
    {
        int successCount = 0;
        vector<int> successListIndex;
        vector<string> successListString;
        char trueChar[10];
        char resultCharArr[100];
        strcpy(trueChar, "true");
         for(int i  = 0; i < resultBuffer.size(); i++)
        {
            if(resultBuffer[i].size() < 1)continue;//忽略掉沒有輸出的

            strcpy(resultCharArr, resultBuffer[i].c_str());
             if(MatchChar(resultCharArr, trueChar) != 1)continue;//忽略掉不是true的

            successListIndex.push_back(i);
            successListString.push_back(outputBuffer[i]);
            successCount++;

        }

        if(mode == PRINT_COUNT)
        {
            return successCount;
        }

        if(mode == PRINT_RESULT)
        {
            if(successCount > 0)
            {
                cout << endl << "-list the success test-" << endl;
                _resultFile << endl << "-list the success test-" << endl;
                for(int i = 0; i < successCount; i++)
                {
                    cout << "the result vector index by " << successListIndex[i] << " = " << successListString[i] << " is success." << endl;
                    _resultFile << "the result vector index by " << successListIndex[i] << " = " << successListString[i] << " is success." << endl;
                }
            }
        }
        return 0;
    }

    //列出失敗的test
    int ListFailureTest(int mode)
    {
        int failureCount = 0;
        vector<int> failureListIndex;
        vector<string> failureListString;
        char falseChar[10];
        char ignoreChar[10];
         char resultCharArr[100];
         char outputCharArr[100];
        strcpy(falseChar, "false");
        strcpy(ignoreChar, "ignore");
         for(int i  = 0; i < resultBuffer.size(); i++)
        {
             if(resultBuffer[i].size() < 1)continue;//忽略掉沒有輸出的

             strcpy(resultCharArr, resultBuffer[i].c_str());
             if(MatchChar(resultCharArr, falseChar) != 1)continue;//忽略掉不是false的

             strncpy(outputCharArr, resultBuffer[i].c_str(), 6);
             outputCharArr[6] = '\0';
             if(MatchChar(outputCharArr, ignoreChar) == 1)continue;//忽略掉是ignore的

            failureListIndex.push_back(i);
            failureListString.push_back(outputBuffer[i]);
            failureCount++;
        }

        if(mode == PRINT_COUNT)
        {
            return failureCount;
        }

        if(mode ==PRINT_RESULT)
        {
            if(failureCount > 0)
            {
                cout << endl << "-list the failure test-" << endl;
                _resultFile << endl << "-list the failure test-" << endl;
                for(int i = 0; i < failureCount; i++)
                {
                    cout << "the result vector index by " << failureListIndex[i] << " = " << failureListString[i] << " is failure." << endl;
                    _resultFile << "the result vector index by " << failureListIndex[i] << " = " << failureListString[i] << " is failure." << endl;
                }
            }
        }
        return 0;
    }

    //列出忽略清單
    int ListIgnoreTest(int mode)
    {
        int ignoreCount = 0;
        vector<int> ignoreListIndex;
        vector<string> ignoreListString;
        char ignoreChar[10];
        char outputCharArr[100];
        strcpy(ignoreChar, "ignore");
        for(int i  = 0; i < resultBuffer.size(); i++)
        {
            if(resultBuffer[i].size() < 1)continue;//忽略掉沒有輸出的

            strncpy(outputCharArr, resultBuffer[i].c_str(), 6);
            outputCharArr[6] = '\0';
//            cout << "outputCharArr = " << outputCharArr << endl;
//            cout << "ignoreChar = " << ignoreChar << endl;

            if(MatchChar(outputCharArr, ignoreChar) != 1)continue;//忽略掉不是ignore的

            ignoreListIndex.push_back(i);
            ignoreListString.push_back(resultBuffer[i]);
            ignoreCount++;
        }

        if(mode ==PRINT_COUNT)
        {
            return ignoreCount;
        }

        if(mode == PRINT_RESULT)
        {
            if(ignoreCount > 0)
            {
                cout << endl << "-list the ignore test-" << endl;
                _resultFile << endl << "-list the ignore test-" << endl;
                for(int i = 0; i < ignoreCount; i++)
                {
                    cout << "the result vector index by " << ignoreListIndex[i] << " is " << ignoreListString[i] << endl;
                    _resultFile << "the result vector index by " << ignoreListIndex[i] << " is " << ignoreListString[i] << endl;
                }
            }
        }

        return 0;
    }

    //對比文字(Char) 回傳1一樣，回傳-1不一樣
    int MatchChar(char *str1, char *str2)//output,test
    {
        int result = 1;
        int charIndex = 0;
        char p1 = str1[charIndex];
        char p2 = str2[charIndex];
        if(p1 == '\0')return -1;
        while(p1 != '\0')
        {
            //cout << "p1 = " << p1 << endl;
            //cout << "p2 = " << p2 << endl;
            if(p1 != p2)return -1;
            if(p2 == '\0')return -1;
           charIndex++;
            p1 = str1[charIndex];
            p2 = str2[charIndex];
        }
        return result;
    }

    //對比文字(Char) 回傳1一樣，回傳-1不一樣，用n個字元
    int MatchNChar(char *str1, char *str2, int n)//output,test
    {
        int result = 1;
        int charIndex = 0;
        char p1 = str1[charIndex];
        char p2 = str2[charIndex];
        if(p1 == '\0')return -1;
        while(p1 != '\0' && charIndex < n)
        {
            //cout << "p1 = " << p1 << endl;
            //cout << "p2 = " << p2 << endl;
            if(p1 != p2)return -1;
            if(p2 == '\0')return -1;
           charIndex++;
            p1 = str1[charIndex];
            p2 = str2[charIndex];
        }
        return result;
    }

    //關閉文字檔
    void Close()
    {
        _inputFile.close();
        _outputFile.close();
        _resultFile.close();
    }

    //輸入檔名，取直到'.'前的字做output
    char* FileNameWithoutType(char* input)
    {
        int charIndex = 0;
        char p1 = input[charIndex];
        char *output;
        while(p1 != '.')
        {
            output[charIndex] = input[charIndex];
            cout << "test charIndex = " << charIndex << endl;
            cout << "output = " << output << endl;
           charIndex++;
           p1 = input[charIndex];
        }
        return output;
    }

    //修改字串eg. 1-1000修改成0001-1000
    char *FillDigit(char *input)
    {
        int digitalShift = 0;
        int zeroShift = 0;
        while(input[digitalShift] != '\0')
            digitalShift++;
        zeroShift = 4 - digitalShift;

        char* output;
        output = new char[5];
        int index = 0;

        for(index = 0; index < zeroShift; index++)
            output[index] = '0';

        for(int digit = 0; digit < digitalShift; digit++)
        {
            output[index] = input[digit];
            index++;
        }
        output[4] = '\0';
        return output;
    }

    //儲存圖片
    void SaveOutputImage(const char* fileName, char* folderPath, Mat& saveMat)
    {
        char savePath[100];
        strcpy(savePath, folderPath);
        strcat(savePath, fileName);
        imwrite(savePath, saveMat);
    }

    //顯示數據
    void PrintResultData()
    {
        int successCount = ListSuccessTest(PRINT_COUNT);
        int failureCount = ListFailureTest(PRINT_COUNT);
        int ignoreCount = ListIgnoreTest(PRINT_COUNT);
        int testCount = failureCount + successCount;
        int totalCount = failureCount + successCount + ignoreCount;
        double successRate = (double)successCount / (double)(testCount);
        double failureRate = (double)failureCount / (double)(testCount);
        cout << "--print the result data--" << endl;
        cout << "the success count = " << successCount << "." << endl;
        cout << "the failure count = " << failureCount << "." << endl;
        cout << "the test count = " << testCount << "(success+failure)." << endl;
        cout << endl;
        cout << "the ignore count = " << ignoreCount << "." << endl;
        cout << "the total count = " << totalCount << "(success+failure+ignore)." <<endl;
        cout << endl;
        cout << "the success rate = " << fixed << setprecision(2) << successRate << "(success/success+failure)." << endl;
        cout << "the failure rate = " << fixed << setprecision(2) << failureRate << "(failure/success+failure)." << endl;
    }

    //寫入數據資料
    void WriteResultData()
    {
        int successCount = ListSuccessTest(PRINT_COUNT);
        int failureCount = ListFailureTest(PRINT_COUNT);
        int ignoreCount = ListIgnoreTest(PRINT_COUNT);
        int testCount = failureCount + successCount;
        int totalCount = failureCount + successCount + ignoreCount;
        double successRate = (double)successCount / (double)(testCount);
        double failureRate = (double)failureCount / (double)(testCount);
        _resultFile << endl << "--print the result data--" << endl;
        _resultFile << "the success count = " << successCount << "." << endl;
        _resultFile << "the failure count = " << failureCount << "." << endl;
        _resultFile << "the test count = " << testCount << "(success+failure)." << endl;
        _resultFile << endl;
        _resultFile << "the ignore count = " << ignoreCount << "." << endl;
        _resultFile << "the total count = " << totalCount << "(success+failure+ignore)." <<endl;
        _resultFile << endl;
        _resultFile << "the success rate = " << fixed << setprecision(2) << successRate << "(success/success+failure)." << endl;
        _resultFile << "the failure rate = " << fixed << setprecision(2) << failureRate << "(failure/success+failure)." << endl;
    }

    //複製檔案
    void CopyFile(const char* inputPath, const char* outputPath)
    {
        ifstream _inputFileCopy;
        ofstream _outputFileCopy;

        _inputFileCopy.open(inputPath, std::ifstream::in);
        _outputFileCopy.open(outputPath , ofstream::out | std::ios::trunc);

        _outputFileCopy << _inputFileCopy.rdbuf();

        _inputFileCopy.close();
        _outputFileCopy.close();
    }

    //儲存input command line
    void SaveCommandLine(char* cammandLineChar)
    {
        _resultFile << endl<< "--the input command line is--";
        _resultFile << endl << cammandLineChar << endl;
    }
};

//////////////////////////////////////////////////////
//以下是沒使用但日後可以參考或擴增的功能//
//////////////////////////////////////////////////////

//    void CopyFile(const char* inputPath, const char* outputPath)
//    {
//        ifstream _inputFileCopy;
//        ofstream _outputFileCopy;
//
//        _inputFileCopy.open(inputPath, std::ifstream::in);
//        _outputFileCopy.open(outputPath , ofstream::out | std::ios::trunc);
//
//        string buffer;
//        char *charBuffer;
//        while(_inputFileCopy >> buffer)
//        {
//            charBuffer = new char[buffer.length() + 1];
//            strcpy(charBuffer, buffer.c_str());
//
//            _outputFileCopy << charBuffer << endl;
//        }
//
//        _inputFileCopy.close();
//        _outputFileCopy.close();
//    }

   //輸出路徑整歸
//    char* ImageOutputPath(char* basePath, char* folderPath, char* fileName)
//    {
//        char* output;
//        output = new char[50];
//        strcpy(output, basePath);
//        strcat(output, folderPath);
//        strcat(output, fileName);
//        return output;
//    }



    //新增以圖片名稱為資料夾名稱的路徑
//    char* AddImageFolderPath(char* oldPath, char* imgName)
//    {
//        char *newPath;
//        newPath = new char[50];
//        strcpy(newPath, oldPath);
//        strcat(newPath, imgName);
//        strcat(newPath, "/");
//        return newPath;
//    }


    //字串比對
//    int MatchString(string str1, string str2)//output,test
//    {
//        int result = 1;
//        int charIndex = 0;
//        char p1 = str1[charIndex];
//        char p2 = str2[charIndex];
//        if(p1 == '\0')return -1;
//        while(p1 != '\0')
//        {
//            //cout << "p1 = " << p1 << endl;
//            //cout << "p2 = " << p2 << endl;
//            if(p1 != p2)return -1;
//            if(p2 == '\0')return -1;
//           charIndex++;
//            p1 = str1[charIndex];
//            p2 = str2[charIndex];
//        }
//        return result;
//    }

    //取得指定index test名稱
//    string GetTestByIndex(int index)
//    {
//        return testBuffer[index];
//    }

    //取得圖片vector大小
//    int GetImgVectorSize()
//    {
//        return imgBuffer.size();
//    }
//
//    //取得test vector大小
//    int GetTestVectorSize()
//    {
//        return testBuffer.size();
//    }
