Inpaint
=======

Examplar Inpainting using OpenCV

Brief Description : An Exemplar based Image Inpainting Algorithm that can be used for object removal from images. The algorithm needs to fill in the target hole region with patches from the Source=Image-Hole region. It needs to perform a continuous loop of finding a hole boundary pixel with highest priority to be filled first , and then select a patch from the source region to be used as Exemplar.

Work done so far :
 - Implemented a working code which can remove objects from input image.
 - Created a modular code for separate white box testing of each major section of the algorithm.
 - Created a Sample which can be used to test the algorithm efficiently on multiple images.
 
Issues that occurred and how were they solved :
   - Earlier used OpenCV Canny Edge Detection to find boundary pixels(fill front). Canny had an advantage of finding the boundary pixels in the form of set of contours . Plus they were in order as well . This helped greatly in easily calculating boundary normal at each boundary pixel , because we approximate normal as the normal vector to the vector formed by boundary pixels ahead and behind of an contour pixel , see this image : 
https://dl.dropboxusercontent.com/u/23755950/gsoc1.JPG

But with canny , boundary was not computed correctly when holes became small (see this image https://dl.dropboxusercontent.com/u/23755950/gsoc2.JPG). An this greatly damaged the output of the inpainting. So I had to implement 2D Laplace kernel filtering for finding boundary. 


   -Many a times a wrong Exemplar was selected for being copied to target region because it had the same PatchError(SSD) as another Correct Exemplar . See this Image :https://dl.dropboxusercontent.com/u/23755950/gsoc3.JPG
This issue was solved by using Variance Calculation in addition to SSD when to Exemplars had same SSD.



Major issues that still needs to be solved :

    -- Using Sobel derivative for calculating isophates appears to be erroneous. It does not well defines edges . See this image : https://dl.dropboxusercontent.com/u/23755950/gsoc4.JPG
I tried thresh-holding the sobel derivative output but that does not really helps . I need to do something like done in Canny algo , like keeping certain long edges only.

    --Once an Exemplar is chosen to be copied to target patch , its isophates (normal gradient) are also copied to the target patch . This sometime leads to unnecessary high isophate values being copied to target patch boundary . This happens when Exemplar Patch boundary has strong isophates at certain pixels. 
I tried to solve this issue by recomputing isophates in every inpaint loop instead of copying them from Exemplar Patch , but this leads to other unnecessary isophates in target region
So now I have to find a way of removing this issue.



Plans for couple of next weeks:
 
-Create an interface for specifying the region in the image that will be used as source region . Currently the whole image is considered to be source . It is always beneficial to search for exemplars near to the target region.

-White Box testing of boundary normal computation. I am afraid my implementation has some errors. 

-Find out solutions for above mentioned two major issues over the internet.
