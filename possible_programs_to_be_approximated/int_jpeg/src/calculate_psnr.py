import numpy as np
import sys
from scipy.ndimage import imread
from scipy.linalg import norm
from scipy import sum, average

# def calculate_psnr(refImage, noisyImage):
    # #Read the dimensions of the image.
    # print size(refImage)
    # # exit(); 
    # # #[rows columns ~] = size(refImage);

    # # #Calculate mean square error of R, G, B.   
    # # mseRImage = (double(refImage(:,:,1)) - double(noisyImage(:,:,1))) .^ 2;
    # # mseGImage = (double(refImage(:,:,2)) - double(noisyImage(:,:,2))) .^ 2;
    # # mseBImage = (double(refImage(:,:,3)) - double(noisyImage(:,:,3))) .^ 2;

    # # mseR = sum(sum(mseRImage)) / (rows * columns);
    # # mseG = sum(sum(mseGImage)) / (rows * columns);
    # # mseB = sum(sum(mseBImage)) / (rows * columns);

    # # #Average mean square error of R, G, B.
    # # mse = (mseR + mseG + mseB)/3;

    # # % Calculate PSNR (Peak Signal to noise ratio).
    # # PSNR_Value = 10 * log10( 255^2 / mse);
    # return PSNR_VALE


def mse(imageAContainer, imageBContainer):
    # the 'Mean Squared Error' between the two images is the
    # sum of the squared difference between the two images;
    # NOTE: the two images must have the same dimension
    err = np.sum((imageAContainer - imageBContainer) **2)
    err /= float(imageA.shape[0] * imageA.shape[1])
    return err

def calculate_psnr(refImage, noisyImage):
    imageRefContainer = imread(refImage)
    #.astype(float)
    noisyImageContainer = imread(noisyImage)
    print imageRefContainer.size
    print imageRefContainer 
    exit()      
    return mse(imageRefPtr, noisyImagePtr)


# ---- testing
def main():
    refImage = "roki.jpg";
    noisyImage = 'roki_2.jpg'
    psnr = calculate_psnr(refImage, noisyImage)
    print "psnr value is" + str(psnr)

if __name__ == "__main__":
    main()
