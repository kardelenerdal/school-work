import java.io.*;
import java.util.Scanner;
public class img {
	public static void main(String[] args) throws FileNotFoundException  {
		int mode = Integer.parseInt(args[0]);
		String inputname = args[1];	
		// ARRAY
			File f = new File (inputname);
			Scanner sc = new Scanner (f);             
			String first = sc.nextLine();             // the image format (P3)
			int columns = sc.nextInt();           
			int rows = sc.nextInt();                  
			int max = sc.nextInt();                   
			sc.nextLine(); 
	
			int [][][] a = new int [rows][columns][3];      
			for(int i=0; i<rows; i++) {
				for(int j=0; j<columns; j++) {
					a[i][j][0]= sc.nextInt();   // 0 red
					a[i][j][1]= sc.nextInt();   // 1 green
					a[i][j][2]= sc.nextInt();   // 2 blue
				}
			}
			sc.close();
				
			if(mode==0) {		
			// SAME IMAGE
			File o = new File ("output.ppm");        
			PrintStream ps = new PrintStream(o);
				 
			toFile(ps,a,first,columns,rows,max);         
			
			}	
			
			else if(mode==1) {
			// BLACK AND WHITE
			int [][][] bw = new int [rows][columns][3];           // this is an array for black and white version.
			for(int i=0; i<rows; i++) {
				for(int j=0; j<rows; j++) {
					int sum = a[i][j][0]+a[i][j][1]+a[i][j][2];   
					int result = sum/3;
					bw[i][j][0]=result;
				    bw[i][j][1]=result;
					bw[i][j][2]=result;
				}
			}		
			
			File sb = new File ("black-and-white.ppm");            // to get the black and white version, there is a new file.
			PrintStream sb2 = new PrintStream(sb);
			
			toFile(sb2,bw,first,columns,rows,max);
			}
			
			else if(mode==2) {
			// FILTER	
			String filtername = args[2];
	
			File filt = new File (filtername);                     
			Scanner sf = new Scanner (filt);
			String area = sf.nextLine();                           // this is the area of the filter file
			int h = Integer.parseInt(area.substring(0, area.indexOf('x')));       // h is the size of the filter file
			int [][] filter = new int [h][h];                      // to make calculations easier, there is an array for filter file.
			for(int i=0; i<h; i++) {
				for(int j=0; j<h; j++) {
					filter[i][j]=sf.nextInt();
				}
			}
			
			int newh = rows-h+1;                                  // new sizes of the filtered array
			int [][][] filtered = new int [newh][newh][3]; 
			
			for(int i=0; i<newh; i++) {							 
				for(int j=0; j<newh; j++) {
					int sum0 = 0;
					int sum1 = 0;
					int sum2 = 0;
					for(int k=0; k<h; k++) {
						for(int m=0; m<h; m++) {
							sum0 = sum0 + a[k+j][m+i][0]*filter[k][m];         
							sum1 = sum1 + a[k+j][m+i][1]*filter[k][m];
							sum2 = sum2 + a[k+j][m+i][2]*filter[k][m];
						}
					}
					if(sum0>max) {          
						sum0 = max;
					} else if (sum0<0) {            
						sum0 = 0;
					}
					if(sum1>max) {
						sum1 = max;
					} else if (sum1<0) {
						sum1 = 0;
					}
					if(sum2>max) {
						sum2 = max;
					} else if (sum2<0) {
						sum2 = 0;
					}
					filtered[i][j][0]= sum0;
					filtered[i][j][1]= sum1;
					filtered[i][j][2]= sum2;
				}
			}
			// after that I apply the black and white part
			for(int i=0; i<newh; i++) {
				for(int j=0; j<newh; j++) {
					int sum = filtered[i][j][0]+filtered[i][j][1]+filtered[i][j][2];
					int result = sum/3;
					filtered[i][j][0]=result;
				    filtered[i][j][1]=result;
					filtered[i][j][2]=result;
				}
			}
			
			
			File fdos = new File ("convolution.ppm");     // new file's name is convolution.
			PrintStream fil = new PrintStream(fdos);
			
			fil.println(first);                          
			fil.println(newh+" "+newh);
			fil.println(max);
			for(int i=0; i<newh; i++) {
				for(int j=0; j<newh; j++) {
					fil.print(filtered[j][i][0]+" ");
					fil.print(filtered[j][i][1]+" ");
					fil.print(filtered[j][i][2]+" ");	
					fil.print("\t");
				}
				fil.println();
			}
			fil.close();
			
			
			}
			
			// QUANTIZATION
			
			else if(mode==3) {
			int range = Integer.parseInt(args[2]);
			// if a number is quantized before, I should not change it so I created a boolean array to memorize the ones that I looked before
			boolean done [][][] = new boolean [columns][columns][3];
			
			for(int color=0; color<3; color++) {  
				for(int row=0; row<a.length; row++) {
					for(int column=0; column<a[row].length; column++) {
						if(done[row][column][color]){     // if that number is done, skip it
							continue;
						}
						quan(a,range,row,column,color,done);    
					}
				}
			}
		
			File col = new File ("quantized.ppm");
			PrintStream co = new PrintStream(col);			
			toFile(co,a,first,columns,rows,max);		
			}
			
	} 
	
	// this method takes an array and fills the file with that array
	public static void toFile(PrintStream filename, int [][][] a,String first,int columns, int rows, int max) {
		filename.println(first);
		filename.println(columns+" "+rows);
		filename.println(max);
		for(int i=0; i<columns; i++) {
			for(int j=0; j<rows; j++) {
				filename.print(a[i][j][0]+" ");
				filename.print(a[i][j][1]+" ");
				filename.print(a[i][j][2]+ " ");	
				filename.print("\t");
			}
			filename.println();
		}
	}
	// this method looks every neighbor of a number and if it is in range, it changes the number and makes the boolean true
	public static int[][][] quan (int [][][] a, int range, int x, int y, int z, boolean [][][] done) {
		done[x][y][z] = true;	
		if(y<a.length-1 && a[x][y+1][z]<=range+a[x][y][z] && a[x][y+1][z]>=a[x][y][z]-range && !done[x][y+1][z]) {
			a[x][y+1][z]=a[x][y][z];   // neighbor on the right
			done[x][y+1][z]=true;
			quan(a,range,x,y+1,z,done);
		} if(y!=0 && a[x][y-1][z]<=range+a[x][y][z] && a[x][y-1][z]>=a[x][y][z]-range && !done[x][y-1][z]) {
			a[x][y-1][z]=a[x][y][z];    // neighbor on the left
			done[x][y-1][z]=true;
			quan(a,range,x,y-1,z,done);
		} if(x<a[x].length-1 && a[x+1][y][z]<=range+a[x][y][z] && a[x+1][y][z]>=a[x][y][z]-range && !done[x+1][y][z]) {
			a[x+1][y][z]=a[x][y][z];      // neighbor above
			done[x+1][y][z]=true;
			quan(a,range,x+1,y,z,done);
		} if(x!=0 && a[x-1][y][z]<=range+a[x][y][z] && a[x-1][y][z]>=a[x][y][z]-range && !done[x-1][y][z]) {
			a[x-1][y][z]=a[x][y][z];      // neighbor below
			done[x-1][y][z]=true;
			quan(a,range,x-1,y,z,done);
		} if(z<2 && a[x][y][z+1]<=range+a[x][y][z] && a[x][y][z+1]>=a[x][y][z]-range && !done[x][y][z+1]) {
			a[x][y][z+1]=a[x][y][z];        // neighbor outer
			done[x][y][z+1]=true;
			quan(a,range,x,y,z+1,done);
		} if(z!=0 && a[x][y][z-1]<=range+a[x][y][z] && a[x][y][z-1]>=a[x][y][z]-range && !done[x][y][z-1]) {
			a[x][y][z-1]=a[x][y][z];       // neighbor inner
			done[x][y][z-1]=true;
			quan(a,range,x,y,z-1,done);
		} 
		
		return a;
	}

}
