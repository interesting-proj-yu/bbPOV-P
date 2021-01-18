#include "SD_MMC.h"
#include "JPEGDEC.h"
#include <NeoPixelBus.h>
JPEGDEC jpeg;
File myfile;

NeoBuffer <NeoBufferMethod<DotStarBgrFeature>> *imagebuffer;
NeoBuffer <NeoBufferMethod<DotStarBgrFeature>> *imagebuffer2;

void * myOpen(const char *filename, int32_t *size) {
  Serial.println("Open");
  myfile = SD_MMC.open(filename);
  *size = myfile.size();
  return &myfile;
}
void myClose(void *handle) {
  if (myfile) myfile.close();
}
int32_t myRead(JPEGFILE *handle, uint8_t *buffer, int32_t length) {
  if (!myfile) return 0;
  return myfile.read(buffer, length);
}
int32_t mySeek(JPEGFILE *handle, int32_t position) {
  if (!myfile) return 0;
  return myfile.seek(position);
}
//uint16_t (*imgBuffer)[80];
// Function to draw pixels to the display
int JPEGDraw(JPEGDRAW *pDraw) {

  //Serial.printf("jpeg draw: x,y=%d,%d, cx,cy = %d,%d\n",pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  //Serial.printf("Before Pixel 80 = 0x%04x\n", pDraw->pPixels[80]);
  int pixels=pDraw->iWidth*pDraw->iHeight;
  for (int i = 0 ;i<=pixels;i++){
      uint8_t b = uint8_t((pDraw->pPixels[i] & 0x001F)<<3); // 5 LSB for blue
      uint8_t g = uint8_t((pDraw->pPixels[i] & 0x07C0)>>3); // 6 'middle' bits for green
      uint8_t r = uint8_t((pDraw->pPixels[i] & 0xF800)>>8); // 5 MSB for red
      imagebuffer->SetPixelColor(pDraw->x+i%80,pDraw->y+i/80,RgbColor(r,g,b));
      imagebuffer2->SetPixelColor(pDraw->x+i%80,pDraw->y+i/80,RgbColor(r,g,b));
    }
  return 1;
}

void setup()
{
  pinMode(2, INPUT_PULLUP);
  Serial.begin(115200);
  //if(imgBuffer = (uint16_t(*)[80]) calloc(80*360,sizeof(uint16_t)))
  //  Serial.println("Alloc memory1 OK");
  NeoBuffer <NeoBufferMethod<DotStarBgrFeature>> imagesb(80, 350, NULL);
  imagebuffer = &imagesb;
  NeoBuffer <NeoBufferMethod<DotStarBgrFeature>> imagesb2(80, 350, NULL);
  imagebuffer2 = &imagesb2;
if(!SD_MMC.begin("/sdcard",true)){
        Serial.println("Card Mount Failed");
    }
    Serial.println("setup done");
} /* setup() */
void loop() {
  long lTime;

  if (jpeg.open("/sb.jpg", myOpen, myClose, myRead, mySeek, JPEGDraw))
  {
    Serial.println("Successfully opened JPEG image");
    Serial.printf("Image size: %d x %d, orientation: %d, bpp: %d\n", jpeg.getWidth(),
      jpeg.getHeight(), jpeg.getOrientation(), jpeg.getBpp());
    if (jpeg.hasThumb())
       Serial.printf("Thumbnail present: %d x %d\n", jpeg.getThumbWidth(), jpeg.getThumbHeight());
    lTime = micros();
    if (jpeg.decode(0,0,0))
    {
      lTime = micros() - lTime;
      Serial.printf("Successfully decoded image in %d us\n", (int)lTime);
    }
    jpeg.close();
  }
 // Serial.printf("After Pixel 80 = 0x%02x\n", imgBuffer[160][50]);
  Serial.printf("R:0x%02x G:0x%02x B:0x%02x\n", imagebuffer->GetPixelColor(50,150).R,imagebuffer->GetPixelColor(50,150).G,imagebuffer->GetPixelColor(50,150).B);
  //Serial.printf(imagebuffer->GetPixelColor(160,50).R);
  Serial.printf("\n\navailable heap in main %i\n", ESP.getFreeHeap());
  Serial.printf("biggest free block: %i\n\”", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
  delay(10000);
}
