package modelutils;

import java.io.FileWriter;
import java.io.IOException;

/**
 * A simple utility FileWriter that provides two new write methods that insert
 * newlines at the end of the text when called
 */
public class MyFileWriter extends FileWriter {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constructor
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Open a file with the provided filename
     *
     * @param fileName The name of the file to open
     * @throws IOException
     */
    public MyFileWriter(String fname) throws IOException {
        super(fname);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // I/O Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Write provided text with newline
     *
     * @param str The text to write to the file
     * @throws IOException
     */
    public void writeln(String str) throws IOException {
        super.write(str);
        super.write(System.lineSeparator());
    }

    /**
     * Write a newline character
     *
     * @throws IOException
     */
    public void writeln() throws IOException {
        super.write(System.lineSeparator());
    }

}
