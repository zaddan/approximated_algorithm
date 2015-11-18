data=csvread('image.csv');
ptr=1;
r=zeros(512,512);
g=r;
b=r;
for i = 1:8:512
    for j=1:8:512
        for k=0:7
            for l=0:7
                
                r(i+k,j+l)=data(ptr,1);
                g(i+k,j+l)=data(ptr,3);
                b(i+k,j+l)=data(ptr,2);
                ptr=ptr+1;
            end
        end
    end
end
rgb=uint8(cat(3,r,g,b));
imshow(rgb);

ref=imread('../obj/lena.bmp');

psnr(rgb,ref)