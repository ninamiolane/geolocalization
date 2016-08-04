import java.awt.image.RenderedImage;

import javax.media.jai.JAI;
import javax.media.jai.RenderedOp;

import com.sun.media.jai.codec.PNMEncodeParam;


public class test {
	
	public static void main(String[] args) {
	String filename = "/Users/JK/Documents/Informatique/jpgtopnm/logo.jpg";

	RenderedImage image = JAI.create("fileload", filename);

    String filename2 = "/Users/JK/Documents/Informatique/jpgtopnm/logoppm.ppm";
    String format = "PNM";
    RenderedOp op = JAI.create("filestore", image,
                               filename2, format);
	}

}
