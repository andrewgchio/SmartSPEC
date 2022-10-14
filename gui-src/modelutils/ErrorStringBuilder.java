package modelutils;

/**
 * A utility class to handle error messages
 */
public class ErrorStringBuilder {
    
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Instance Variables
    ////////////////////////////////////////////////////////////////////////////

    /**
     * The underlying StringBuilder that will buffer messages
     */
    private StringBuilder builder = new StringBuilder();

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Queries / Access Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return a string representing the data in the StringBuilder
     * 
     * @return A string representing the data in the StringBuilder
     */
    public String toString() {
        return builder.toString();
    }

    /**
     * Return true if the StringBuilder contents are empty
     * 
     * @return True if the StringBuilder contents are empty
     */
    public boolean isEmpty() {
        return builder.length() == 0;
    }

    /**
     * Appends "Error: {msg}\n"
     * 
     * @param msg  The error message
     * @return A reference to this object
     */
    public ErrorStringBuilder append(String msg) {
        builder.append("Error: ").append(msg).append(System.lineSeparator());
        return this;
    }

    /**
     * Appends "Error: {msg} {disp}\n"
     * 
     * @param msg  The error message
     * @param disp A display name
     * @return A reference to this object
     */
    public ErrorStringBuilder append(String msg, String disp) {
        builder.append("Error: ")
               .append(msg)
               .append(" ")
               .append(disp)
               .append(System.lineSeparator());
        return this;
    }

}
