import numpy as np
from PIL import Image
from PIL import ImageFile
import math 
ImageFile.LOAD_TRUNCATED_IMAGES = True

# ---- get the RGB values of an image
def get_RGB_values(imageName):
    im = Image.open(imageName)
    pix = im.load()
    RValues = []
    GValues = []
    BValues = []
    for i in range(im.size[0]):
        for j in range(im.size[1]):
            RValues.append(pix[i, j][0])
            GValues.append(pix[i, j][1])
            BValues.append(pix[i, j][2])

    return(RValues, GValues, BValues, im.size)

# ---- calculate the psnr of two images
def calculate_psnr(refImage, noisyImage):
    # ---- get R,G,B values of the two image
    refImageRVal, refImageGVal, refImageBVal, img_size = get_RGB_values(refImage)
    noisyImageRVal, noisyImageGVal, noisyImageBVal,_  = get_RGB_values(noisyImage)
    
    # ---- calculate the mean squared error
    img_total_size = img_size[0]*img_size[1] 
    mseR = np.sum((np.asarray(refImageRVal) - np.asarray(noisyImageRVal)) **2)/img_total_size
    mseG = np.sum((np.asarray(refImageGVal) - np.asarray(noisyImageGVal)) **2)/img_total_size
    mseB = np.sum((np.asarray(refImageBVal) - np.asarray(noisyImageBVal)) **2)/img_total_size
    
    # ---- Average mean square error of R, G, B.
    mse = (mseR + mseG + mseB)/3;

    # ---  Calculate PSNR (Peak Signal to noise ratio).
    PSNR_Value = 10 * math.log10( 255^2 / mse);
    return PSNR_Value


#------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# # ---- testing
# def main():
    # refImage = "roki.jpg";
    # noisyImage = 'roki_2.jpg'
    # psnr = calculate_psnr(refImage, noisyImage)
    # print psnr

# if __name__ == "__main__":
#     main()
