from skimage import morphology
import cv2
import sys

if len(sys.argv) < 3:
    print "this-script input-image output-image"
    quit()

im = cv2.imread(sys.argv[1])
im = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
im = cv2.threshold(im, 128, 255, cv2.THRESH_BINARY_INV)[1]
im = morphology.skeletonize(im > 0)
im = im * 255
cv2.imwrite(sys.argv[2], im)
