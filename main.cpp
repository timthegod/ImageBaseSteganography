//
//  main.cpp
//  IP_partD_decoder
//
//  Created by 楊廷禹 on 2019/1/4.
//  Copyright © 2019 Yang Ting Yu. All rights reserved.
//

#include <iostream>
#include <opencv.hpp>

using namespace cv;
using namespace std;

unsigned long Hash(string);
void getRandomPointAndChannelArray(int , int , Point2i [], int[], RNG );
void decode(int, int, Mat &, Mat, Point2i[], int[]);

int main(int argc, const char * argv[]) {
//    if(argc != 5){
//        cerr<<"Usage: " << argv[0] << " <colour carrier path>" <<" <encoded message image path>"<<" <path for storing randomed message>"<<" <path for storing decoded message>"<< endl;
//        return -1;
//    }
    Mat srcColor, srcEncoded;
    Mat messageRandom;
    String srcColorPath = "colour_carrier.png";
    String srcEncodedPath = "outputFromEncoder.png";
    String randomMessageWritePath = "messageRandom.png";
    String dstWritePath = "messageDecoded.png";
    
    String srcColorWindow = "colour_carrier";
    String srcEncodedWindow = "output";
    String randomMessageWindow = "Message Randomed";
    String dstWindow = "Message";
    
    
    //read carrier and message image
    srcColor = imread(srcColorPath, -1);
    srcEncoded = imread(srcEncodedPath, -1);
    if (srcColor.empty() || srcEncoded.empty()) {
        cerr<<"Image loading error!"<<endl;
    }
    Mat dst = Mat(srcEncoded.size(),CV_8U);
    namedWindow(srcColorWindow, CV_WINDOW_AUTOSIZE);
    namedWindow(srcEncodedWindow, CV_WINDOW_AUTOSIZE);
    
    imshow(srcColorWindow, srcColor);
    imshow(srcEncodedWindow, srcEncoded);
    
    string password;
    cout<<"Enter 8 char password: ";
    cin>>password;
    
    RNG rng(Hash(password));
    Point2i randomPoint[srcEncoded.rows * srcEncoded.cols];
    //to store channel selection order
    int selectChannelOrder[srcEncoded.rows * srcEncoded.cols];
    
    getRandomPointAndChannelArray(srcColor.rows, srcColor.cols, randomPoint, selectChannelOrder, rng);
    
    absdiff(srcEncoded, srcColor, messageRandom);
    
    
    
    for (int i = 0; i < messageRandom.rows; i++) {
        for(int j = 0; j < messageRandom.cols; j++){
            int x = randomPoint[i * srcEncoded.cols + j].x;
            int y = randomPoint[i * srcEncoded.cols + j].y;
            int selectChannel = selectChannelOrder[i * messageRandom.cols + j];
            int value = messageRandom.at<Vec3b>(x, y)[selectChannel];
            dst.at<uchar>(i, j) = value;
        }
    }
    
    int cnt = 0; //For channel order
    for (int i = 0; i < messageRandom.rows; i++) {
        for (int j = 0; j < messageRandom.cols; j++) {
            int selectChannel = selectChannelOrder[cnt];
            if(messageRandom.at<Vec3b>(i, j)[selectChannel] == 1){
                messageRandom.at<Vec3b>(i, j)[selectChannel] = 0;
            }
            else{
                messageRandom.at<Vec3b>(i, j)[0] = 255;
                messageRandom.at<Vec3b>(i, j)[1] = 255;
                messageRandom.at<Vec3b>(i, j)[2] = 255;
            }
            cnt++;
        }
    }
    
    for (int i = 0; i < dst.rows; i++) {
        for (int j = 0; j < dst.cols; j++) {
            if(dst.at<uchar>(i, j) == 1){
                dst.at<uchar>(i, j) = 0;
            }
            else{
                dst.at<uchar>(i, j) = 255;
            }
        }
    }
    
    namedWindow(dstWindow, CV_WINDOW_AUTOSIZE);
    imshow(dstWindow, dst);
    
    namedWindow(randomMessageWindow, CV_WINDOW_AUTOSIZE);
    imshow(randomMessageWindow, messageRandom);
    
    imwrite(randomMessageWritePath, messageRandom);
    imwrite(dstWritePath, dst);
    cout<<"Image stored"<<endl;
    waitKey(0);
    return 0;
}

unsigned long Hash(string str)
{
    unsigned long hash = 5381;
    
    for (int i = 0; i < str.length(); i++) {
        hash = ((hash << 5) + hash) + str[i]; /* hash * 33 + c */
    }
    cout<<"hash: "<<hash<<endl;
    return hash;
}

void getRandomPointAndChannelArray(int R, int C, Point2i random[], int channel[], RNG rng)
{
    for (int i = 0; i < R; i++) {
        for (int j = 0 ; j < C; j++) {
            random[i * C + j] = Point2i(i,j);
            channel[i * C + j] =rng.operator()(3);
        }
    }
    //swap point by RNG
    for (int i = 0; i < R * C; i++) {
        int order = rng.operator()(R * C);
        swap(random[i],random[order]);
        swap(channel[i],channel[order]);
    }
}

void decode(int R, int C, Mat& dst, Mat randomMessage, Point2i random[], int channel[])
{
    for (int i = 0; i < R; i++) {
        for(int j = 0; j < C; j++){
            int x = random[i * C + j].x;
            int y = random[i * C + j].y;
            int selectChannel = channel[i * C + j];
            int value = randomMessage.at<Vec3b>(x, y)[selectChannel];
            dst.at<uchar>(i, j) = value;
        }
    }
    
    int cnt = 0; //For channel order
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            int selectChannel = channel[cnt];
            if(randomMessage.at<Vec3b>(i, j)[selectChannel] == 1){
                randomMessage.at<Vec3b>(i, j)[selectChannel] = 0;
            }
            else{
                randomMessage.at<Vec3b>(i, j)[0] = 255;
                randomMessage.at<Vec3b>(i, j)[1] = 255;
                randomMessage.at<Vec3b>(i, j)[2] = 255;
            }
            cnt++;
        }
    }
    
    for (int i = 0; i < dst.rows; i++) {
        for (int j = 0; j < dst.cols; j++) {
            if(dst.at<uchar>(i, j) == 1){
                dst.at<uchar>(i, j) = 0;
            }
            else{
                dst.at<uchar>(i, j) = 255;
            }
        }
    }
}
