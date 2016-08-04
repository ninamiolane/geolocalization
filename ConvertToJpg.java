import java.awt.Image;
import java.awt.image.BufferedImage;
import java.awt.image.RenderedImage;
import java.io.File;
import java.io.FileOutputStream;

import javax.imageio.ImageIO;
import javax.media.jai.JAI;
import javax.media.jai.RenderedOp;

import com.sun.media.jai.codec.ImageCodec;
import com.sun.media.jai.codec.ImageEncoder;
import com.sun.media.jai.codec.JPEGEncodeParam;
import com.sun.media.jai.codec.PNMEncodeParam;

public class ConvertToJpg {
 
	public static void main(String args[])
	{
		try{
		String inputFilePath = "/Users/JK/Documents/Informatique/jpgtopnm/test000.pgm";
		String outputFilePath = "/Users/JK/Documents/Informatique/jpgtopnm/test000.jpg";
	
		File inputFile = new File(inputFilePath);
	//	int x = ImageIO.read(inputFile).getWidth();
	//	int y = ImageIO.read(inputFile).getHeight();
	//	int y2 = (int) y*400/x;
	//	int x2 = 400;
		RenderedImage img = JAI.create("fileload", inputFilePath);
		
		/* Image image = ImageIO.read(inputFile).getScaledInstance(500, 500, Image.SCALE_SMOOTH);
		BufferedImage bufferedImage= new BufferedImage(500, 500, BufferedImage.TYPE_BYTE_GRAY);
 
		bufferedImage.createGraphics().drawImage(image, 0, 0, null);
		
		FileOutputStream fileOutputStream = new FileOutputStream(outputFilePath);
 		*/
		JPEGEncodeParam param = new JPEGEncodeParam();
	//	param.setRaw(true);
		RenderedOp op = JAI.create("filestore", img,
                outputFilePath, "JPEG");
		/*
		ImageEncoder encoder = ImageCodec.createImageEncoder("JPEG", fileOutputStream, param);
 
		encoder.encode(bufferedImage);
		
		fileOutputStream.close();
 		*/
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
	}
}