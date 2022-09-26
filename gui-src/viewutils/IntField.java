package viewutils;

import javafx.scene.control.TextField;

/**
 * A text field that only accepts integers
 */
public class IntField extends TextField {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constants
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Width of the IntField text box
     */
    public static final int NUM_WIDTH = 50;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Construct an IntField object
     */
    public IntField() {
        super();

        this.setMaxWidth(NUM_WIDTH);
    }
    
    /**
     * Construct an IntField object with default value
     * @param i An integer value
     */
    public IntField(Integer i) {
        super(i.toString());

        this.setMaxWidth(NUM_WIDTH);
    }
    
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Getter Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return an integer representing the input integer value
     *
     * @return The input integer
     */
    public Integer getValue() {
        try {
            return Integer.parseInt(getText());
        }
        catch (NumberFormatException e) {
            throw new IllegalArgumentException("Invalid integer: " + getText());
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Setter Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Set the integer
     *
     * @param x The integer to set
     */
    public void setValue(Integer x) {
        if (x != null)
            setText(x.toString());
    }

}