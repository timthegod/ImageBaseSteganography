//
//  main.cpp
//  IP_partD_encoder
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
void encode(int, int, Mat&, Mat, Point2i [], int[], int&);

int main(int argc, const char * argv[]) {
//    if(argc != 4){
//        cerr<<"Usage: " << argv[0] << " <colour carrier path>" <<" <message image path>"<<" <path for storing encoded message>"<< endl;
//        return -1;
//    }
    Mat srcColor, srcBinary, dst;
    String srcColorPath = "colour_carrier.png";
    String srcBinaryPath = "message6.png";
    String dstWritePath = "outputFromEncoder.png";
    
    String srcColorWindow = "grayscale_carrier";
    String srcBinaryWindow = "message";
    String dstWindow = "output";
    
    
    //read carrier and message image
    srcColor = imread(srcColorPath, -1);
    srcBinary = imread(srcBinaryPath, -1);
    if(srcColor.empty() || srcBinary.empty()){
        cerr<<"Image loading error!"<<endl;
    }
    
    //clone srcGray to dst for output
    dst = srcColor.clone();
    
    namedWindow(srcColorWindow, CV_WINDOW_AUTOSIZE);
    namedWindow(srcBinaryWindow, CV_WINDOW_AUTOSIZE);
    
    imshow(srcColorWindow, srcColor);
    imshow(srcBinaryWindow, srcBinary);
    
    string password;
    cout<<"Set 8 char password: ";
    cin>>password;
    
    RNG rng(Hash(password));
    Point2i randomPoint[srcColor.rows * srcColor.cols];
    int channelOrder[srcColor.rows * srcColor.cols];
    
    getRandomPointAndChannelArray(srcColor.rows, srcColor.cols, randomPoint, channelOrder, rng);

    int overFlow = 0;
    encode(srcColor.rows, srcColor.cols, dst, srcBinary, randomPoint, channelOrder, overFlow);
    
    cout<<"over: "<<overFlow<<endl;
    namedWindow(dstWindow, CV_WINDOW_AUTOSIZE);
    imshow(dstWindow, dst);
    
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
    cout<<hash<<endl;
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

void encode(int R, int C, Mat& dst, Mat bin, Point2i random[], int channel[], int &over)
{
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            if(bin.at<uchar>(i, j) == 0){
                int x = random[i * C + j].x;
                int y = random[i * C + j].y;
                int selectChannel = channel[i * C + j];
                int value = dst.at<Vec3b>(x, y)[selectChannel];
                if(value != 255){
                    dst.at<Vec3b>(x, y)[selectChannel] += 1;
                }
                else{
                    dst.at<Vec3b>(x, y)[selectChannel] -= 1;
                    over++;
                }
            }
        }
    }
}
