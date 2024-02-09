#include "graphics.h"

/*
    FOR DOCUMENTATION PURPOSES, WHEN LINES ARE 720 AND COLUMNS ARE 1424
    -------------------------------------------------------------------
    // ////////////////////////////////////////////////////////////////////////////
    //  Horizontal line and a vertical line. All other lines are between two lines
    // ////////////////////////////////////////////////////////////////////////////
  */
 /*
    //unsafe { draw_line(self.idat.data.as_mut_ptr(), self.height.try_into().unwrap(), self.width.try_into().unwrap(), 205, 200, 2000, 200, 1) };
    //unsafe { draw_line(self.idat.data.as_mut_ptr(), self.height.try_into().unwrap(), self.width.try_into().unwrap(), 500, 200, 1000, 700, 1) };
  */
EXPORT_IMPORT void draw_line(unsigned char* pixels, unsigned int height, unsigned int width, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, PIXEL pixel)
{
    /*
    x1 = x1*(-1);
    x2 = x2*(-1);
     */
    // Steep is Vertically inclined
    unsigned char steep = (abs(y2 - y1) > abs(x2 - x1)) ? 1 : 0;
    // d is for direction. The directions are two, move to right, or move to right and downwards
    int d, ystep;
    /*unsigned*/ int tmp, dx, dy, x, y;

    // When line is steep(vertically inclined), make it horizontally inclined(less steep)    
    if (steep == 1)
    {        
        // swap(x1, y1)
        tmp = y1;
        y1 = x1;
        x1 = tmp;

        // swap(x2, y2)
        tmp = y2;
        y2 = x2;
	    x2 = tmp;  
    }
    
    //We always move from left to right(that is x is always incremented). For that x1 has to be closer to origin on x-axis than x2
    if ( x1 > x2 ) 
    {	  
	    //swap(x1, x2);	
	    tmp = x2;
	    x2 = x1;
	    x1 = tmp;
		
	    //swap(y1, y2)	
	    tmp = y2;
	    y2 = y1;
	    y1 = tmp;
    }

    // Change in x and change in y. Change in x cant be negative because we aready made sure that we would always move from left to right
    dx = x2 - x1; 	  
    dy = abs(y2 - y1);

    //Initial value, the first and the last points are always on the line, so error is zero(2e=2(0)=0)
    //e = dyX - dxY + c
    //eR = dy(X + 1) - dxY + c = e + dy
    //eD = dy(X + 1) - dx(Y + 1) + c = e + dy - dx
    //d = eR + eD
    //d = e + dy + e + dy - dx
    //d = 2e + 2dy - dx
    // The first and the last points are always on the line, so error is zero(2e=2(0)=0) hence...     
    d = 2*dy - dx;

    if ( y1 < y2 ) 
    {
	    ystep = 1;
    }
    else
    {
        ystep = -1;
    }

    //Initial values(initial ordinate pair)		 
    x = x1;
    y = y1;

    while ( x <= x2 ) 
    {
        //x is reflected as y(transitive) 		 
	    if ( steep == 1 )
        {
	        //cxt.fillRect(y, x, 1, 1);
	        //setPixelColor(cxt, y, x, color);
            //pixels[x*(width*DEFAULT_CPAINT_BIT_DEPTH) + (/*(width - 1) -*/ (y*DEFAULT_CPAINT_BIT_DEPTH))] = color;

            pixels[x*(width*DEFAULT_CPAINT_BIT_DEPTH) + (y*DEFAULT_CPAINT_BIT_DEPTH) + 0] = pixel.b;
            pixels[x*(width*DEFAULT_CPAINT_BIT_DEPTH) + (y*DEFAULT_CPAINT_BIT_DEPTH) + 1] = pixel.g;
            pixels[x*(width*DEFAULT_CPAINT_BIT_DEPTH) + (y*DEFAULT_CPAINT_BIT_DEPTH) + 2] = pixel.r;

            /*pixels[x*(width*DEFAULT_CPAINT_BIT_DEPTH) + (y*DEFAULT_CPAINT_BIT_DEPTH) + 3] = pixel.a;*/
	    }  
	    else
        {
            //cxt.fillRect(x, y, 1, 1);	
            //setPixelColor(cxt, x, y, color); 	  
            //pixels[y*(width*DEFAULT_CPAINT_BIT_DEPTH) + (/*(width - 1) -*/ (x*DEFAULT_CPAINT_BIT_DEPTH))] = color;

            pixels[y*(width*DEFAULT_CPAINT_BIT_DEPTH) + (x*DEFAULT_CPAINT_BIT_DEPTH) + 0] = pixel.b;
            pixels[y*(width*DEFAULT_CPAINT_BIT_DEPTH) + (x*DEFAULT_CPAINT_BIT_DEPTH) + 1] = pixel.g;
            pixels[y*(width*DEFAULT_CPAINT_BIT_DEPTH) + (x*DEFAULT_CPAINT_BIT_DEPTH) + 2] = pixel.r;

            /*pixels[y*(width*DEFAULT_CPAINT_BIT_DEPTH) + (x*DEFAULT_CPAINT_BIT_DEPTH) + 3] = pixel.a;*/
	    }  

        //We only allow two moves, move to the right, or move diagonally(move to the right and downwards). when we move to the right we only increment x otherwise we increment both(sign of ystep)
	    if ( d < 0 ) 
        {   
            d = d + 2*dy;
        }
        else 
        {		    
	        d = d + 2*dy - 2*dx;
            y = y + ystep;			
        }  

        x = x + 1; 	    
    }    

    /*
    for (unsigned int i = 0; i < height; i++)
    {
        unsigned char* line = malloc(width*(sizeof(unsigned char)));

        for (unsigned int j = 0; j < width; j++)
        {
            line[j] = pixels[i*width + ((width - 1) - j)];
        }

        for (unsigned int j = 0; j < width; j++)
        {
            pixels[i*width + j ] = line[j];
        }

        free(line);
    }
     */
}

EXPORT_IMPORT void set_back_or_foreground_color(unsigned char* ptr, unsigned long long n, PIXEL pixel)
{
    
    for (unsigned long long i = 0; i < n; i=i+sizeof(PIXEL)) 
    {

        ptr[i + 0] = pixel.b;
        ptr[i + 1] = pixel.g;
        ptr[i + 2] = pixel.r;

        /*ptr[i + 3] = pixel.a;*/
    }
}