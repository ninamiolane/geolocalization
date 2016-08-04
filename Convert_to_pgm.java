import javax.imageio.ImageIO;
import java.awt.Image;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileOutputStream;
 
import com.sun.media.jai.codec.PNMEncodeParam;
import com.sun.media.jai.codec.ImageEncoder;
import com.sun.media.jai.codec.ImageCodec;
 
public class Convert {
 
	public static void main(String args[])
	{
		try{
		String inputFilePath = "/Users/JK/Documents/Informatique/jpgtopnm/test2.jpg";
		String outputFilePath = "/Users/JK/Documents/Informatique/jpgtopnm/test2.pgm";
	
		File inputFile = new File(inputFilePath);
		int x = ImageIO.read(inputFile).getWidth();
		int y = ImageIO.read(inputFile).getHeight();
		int y2 = (int) y*400/x;
		int x2 = 400;
		Image image = ImageIO.read(inputFile).getScaledInstance(x2, y2, Image.SCALE_SMOOTH);
		BufferedImage bufferedImage= new BufferedImage(x2, y2, BufferedImage.TYPE_BYTE_GRAY);
 
		bufferedImage.createGraphics().drawImage(image, 0, 0, null);
		
		FileOutputStream fileOutputStream = new FileOutputStream(outputFilePath);
 
		PNMEncodeParam param = new PNMEncodeParam();
		param.setRaw(true);
 
		ImageEncoder encoder = ImageCodec.createImageEncoder("PNM",	fileOutputStream,	param);
 
		encoder.encode(bufferedImage);
		
		fileOutputStream.close();
 
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
	}
}
