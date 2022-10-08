
#include "bitmap.h"

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <errno.h>
#include <string.h>
struct T{
           
               struct bitmap *bm;
               double xmin;
               double xmax;
               double ymin;
               double ymax;
               int    max;
               int   counter;
               int   num;

               }compute_args;

int iteration_to_color( int i, int max );
int iterations_at_point( double x, double y, int max );
void *compute_image( struct T *compute_args );

void show_help()
{
	printf("Use: mandel [options]\n");
	printf("Where options are:\n");
	printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
	printf("-x <coord>  X coordinate of image center point. (default=0)\n");
	printf("-y <coord>  Y coordinate of image center point. (default=0)\n");
	printf("-s <scale>  Scale of the image in Mandlebrot coordinates. (default=4)\n");
	printf("-W <pixels> Width of the image in pixels. (default=500)\n");
	printf("-H <pixels> Height of the image in pixels. (default=500)\n");
	printf("-o <file>   Set output file. (default=mandel.bmp)\n");
	printf("-h          Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
	printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
	printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}

int main( int argc, char *argv[] )
{
	char c;

	// These are the default configuration values used
	// if no command line arguments are given.

	const char *outfile = "mandel.bmp";
	double xcenter = 0;
	double ycenter = 0;
	double scale = 4;
	int    image_width = 500;
	int    image_height = 500;
	int    max = 1000;
	int    n=1;
    int    i=0;
	struct timeval begin_time;
	struct timeval end_time;
	// fetching start time
	gettimeofday( &begin_time, NULL );

	// For each command line argument given,
	// override the appropriate configuration value.

	while((c = getopt(argc,argv,"x:y:s:W:H:m:o:num:h"))!=-1) {
		switch(c) {
			case 'x':
				xcenter = atof(optarg);
				break;
			case 'y':
				ycenter = atof(optarg);
				break;
			case 's':
				scale = atof(optarg);
				break;
			case 'W':
				image_width = atoi(optarg);
				break;
			case 'H':
				image_height = atoi(optarg);
				break;
			case 'm':
				max = atoi(optarg);
				break;
			case 'o':
				outfile = optarg;
				break;
            case 'num':
                n=atof(optarg);
                break;
			case 'h':
				show_help();
				exit(1);
				break;
		}
	}

	      struct T compute_args;
          compute_args.bm=bitmap_create(image_width,image_height);
          compute_args.xmin=xcenter-scale;
          compute_args.xmax=xcenter+scale;
          compute_args.ymin=ycenter-scale;
          compute_args.ymax=ycenter+scale;
          compute_args.max=max;
          compute_args.num=n;
           
           pthread_t pt[n];
        // pt=(pthread_t)malloc(sizeof(pthread_t)*num);
	// Display the configuration of the image.
	printf("mandel: x=%lf y=%lf scale=%lf max=%d outfile=%s\n",xcenter,ycenter,scale,max,outfile);

	// Create a bitmap of the appropriate size.
	//struct bitmap *bm = bitmap_create(image_width,image_height);

	// Fill it with a dark blue, for debugging
	//bitmap_reset(compute_args.bm,MAKE_RGBA(0,0,255,0));

	// Compute the Mandelbrot image
          for(i=0;i<n;i++){
            
             compute_args.counter=i;            

          if(pthread_create(&pt[i],NULL,compute_image,&compute_args)){
            
            perror("Error creating thread: ");
            exit( EXIT_FAILURE );
          }
            
           
	   //compute_image(bm,xcenter-scale,xcenter+scale,ycenter-scale,ycenter+scale,max);
         }
          for(i=0;i<n;i++){
             
          if(pthread_join(pt[i],NULL)){
             
              perror("Problem with pthread_join: "); 
           }

            }
         
	// Save the image in the stated file.
	if(!bitmap_save(compute_args.bm,outfile)) {
		fprintf(stderr,"mandel: couldn't write to %s: %s\n",outfile,strerror(errno));
		return 1;
	}
		// fetching end time
	gettimeofday( &end_time, NULL );	
	// calculate total time to execute this code
	long time_to_execute = ( end_time.tv_sec * 1000000 + end_time.tv_usec ) -
	                     ( begin_time.tv_sec * 1000000 + begin_time.tv_usec );	
	printf("This code took %d microseconds to execute\n", time_to_execute);	

       	return 0;
}

void *compute_image( struct T *ptargs )
{       
       
       if((ptargs->counter)>=(ptargs->num))

           exit(0);

	int i,j,p;

	int width = bitmap_width(ptargs->bm);
      
	int height = bitmap_height(ptargs->bm);
    
        
         if((ptargs->counter)==((ptargs->num)-1)){
             
            p=height;
               
             }

         else{
         
            p=(height/ptargs->num)*(ptargs->counter+1);
       
             }
      
         j=j*(ptargs->counter);

	// For every pixel in the image...
       
	while(j<p) {

		for(i=0;i<width;i++) {

			// Determine the point in x,y space for that pixel.
			double x = ptargs->xmin + i*(ptargs->xmax-ptargs->xmin)/width;
			double y = ptargs->ymin + j*(ptargs->ymax-ptargs->ymin)/height;
                        

			// Compute the iterations at that point.
			int iters = iterations_at_point(x,y,ptargs->max);
                      

			// Set the pixel in the bitmap.
			bitmap_set(ptargs->bm,i,j,iters);
                          
		}
            
           j++;
           
	}
    return 0;
}

/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point( double x, double y, int max )
{
	double x0 = x;
	double y0 = y;

	int iter = 0;

	while( (x*x + y*y <= 4) && iter < max ) {

		double xt = x*x - y*y + x0;
		double yt = 2*x*y + y0;

		x = xt;
		y = yt;

		iter++;
	}

	return iteration_to_color(iter,max);
}

/*
Convert a iteration number to an RGBA color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/

int iteration_to_color( int i, int max )
{
	int gray = 255*i/max;
	return MAKE_RGBA(gray,gray,gray,0);
}
