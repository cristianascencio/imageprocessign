#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;		//a computer vision library 

float distance(int x, int y, int i, int j) {
    return float(sqrt(pow(x - i, 2) + pow(y - j, 2))); // regular distance formula between two coordiantes
}

double gaussian(float x, double sigma) {
    return exp(-(pow(x, 2))/(2 * pow(sigma, 2))) / (2 * CV_PI * pow(sigma, 2)); // outputting gaussian based on a mean and variance

}

void applyBilateralFilter(Mat source, Mat filteredImage, int x, int y, int diameter, double sigmaI, double sigmaS) {
    double iFiltered = 0;
    double wP = 0;
    int neighbor_x = 0;
    int neighbor_y = 0;
    int half = diameter / 2;		//radius 
		
  	// m refers to the coordinates of the neighbors, while i refers to the original x and y
    for(int i = 0; i < diameter; i++) {        // this part iterates
        for(int j = 0; j < diameter; j++) {    // through circular neighbors of x and y
            neighbor_x = x - (half - i);
            neighbor_y = y - (half - j);
            double gi = gaussian(source.at<uchar>(neighbor_x, neighbor_y) - source.at<uchar>(x, y), sigmaI); // gaussian of the I(m) - I(i)
            double gs = gaussian(distance(x, y, neighbor_x, neighbor_y), sigmaS);			//one gaussian is taking the distance of the neighbor as the mean
            double w = gi * gs; //  creating G(||(m - i)||)*H(I(m)-I(i)) //look at module 7 
            iFiltered = iFiltered + source.at<uchar>(neighbor_x, neighbor_y) * w; //accumulating I(m)*G(||(m - i)||)*H(I(m)-I(i))
            wP = wP + w; // accumulating the values of G(||(m - i)||)*H(I(m)-I(i)), which is 1/(Kh * Kg)
        }
    }
    iFiltered = iFiltered / wP; // dividing by the accumulated Kh and Kg to get unit area and volume, refer to slides 7 and 8
    filteredImage.at<double>(x, y) = iFiltered; // setting the corresponding filtered value in the new filtered image


}

Mat bilateralFilterOwn(Mat source, int diameter, double sigmaI, double sigmaS) {
    Mat filteredImage = Mat::zeros(source.rows,source.cols,CV_64F); // creates same size image as source filled with zeroes
    int width = source.cols;
    int height = source.rows;
		
  	
    for(int i = 2; i < height - 2; i++) {    // weird indexing, looks like its avoiding edges but start indexes should be 1
        for(int j = 2; j < width - 2; j++) { // either way, iterate through image and filter each pixel
            applyBilateralFilter(source, filteredImage, i, j, diameter, sigmaI, sigmaS);
        }
    }
    return filteredImage;
}


int main(int argc, char** argv ) {
    Mat src;		//Mat is a container for images, its a class, it contains 2 parts, header (info like dimensions and how its store) and pointer to array of pixels
    src = imread( argv[1], 0 );	//reading the image into the src Matrix variable
    imwrite("original_image_grayscale.png", src); //saving the inputted image into a png format, called "original_image_grayscale.png"

    if ( !src.data )						//src.data is a pointer to the actual matrix data, if it's NULL then the image data doesn't exist
    {
        printf("No image data \n");
        return -1;
    }

    Mat filteredImageOpenCV;		//this is  going to be our filtered image
    bilateralFilter(src, filteredImageOpenCV, 5, 12.0, 16.0);		//built in function of bilateral filtering
    imwrite("filtered_image_OpenCV.png", filteredImageOpenCV);	//saving the variable into a picture file
  
    Mat filteredImageOwn = bilateralFilterOwn(src, 5, 12.0, 16.0);	//custom bilateral function
    imwrite("filtered_image_own.png", filteredImageOwn);

    return 0;
}
