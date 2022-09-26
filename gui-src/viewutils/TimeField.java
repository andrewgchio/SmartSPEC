package viewutils;

import java.time.LocalTime;
import java.time.format.DateTimeFormatter;

import javafx.scene.control.TextField;

/**
 * A text field that only accepts a time of format HH:mm or HH:mm:ss
 */
public class TimeField extends TextField {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constants
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Width of TimeField text box
     */
    public static final int TIME_WIDTH = 70;

    /**
     * Output time format as string "HH:mm:ss"
     */
    public final static String FORMAT = "HH:mm:ss";

    /**
     * Formatter that follows the pattern "HH:mm:ss"
     */
    public final static DateTimeFormatter FORMATTER =
            DateTimeFormatter.ofPattern(FORMAT);

    /**
     * Formatter that follows the ISO format for time
     */
    public final static DateTimeFormatter LAZY_FORMATTER =
            DateTimeFormatter.ISO_LOCAL_TIME;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Construct a TimeField object
     */
    public TimeField() {
        super();

        this.setMaxWidth(TIME_WIDTH);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Getter Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return the input time as a string. 0 will be prepended to the hour if it
     * is given as a single digit.
     *
     * @return A string representing the input time
     */
    public String getValue() {
        // Fix single digit hours in format X:XX or X:XX:XX
        if (getText().length() == 4 || getText().length() == 6)
            setText("0" + getText());// fix single digit hours

        return LocalTime.parse(getText(), LAZY_FORMATTER).format(FORMATTER);
    }

    /**
     * Return the input time as a LocalTime object. 0 will be prepended to the
     * hour if it is given as a single digit.
     *
     * @return A LocalTime object representing the input time
     */
    public LocalTime getTimeValue() {
        // Fix single digit hours in format X:XX or X:XX:XX
        if (getText().length() == 4 || getText().length() == 6)
            setText("0" + getText());

        return LocalTime.parse(getText(), LAZY_FORMATTER);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Setter Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Set the time string
     *
     * @param text A string representing the time to set
     */
    public void setValue(String text) {
        setText(LocalTime.parse(text, LAZY_FORMATTER).format(FORMATTER));
    }

    /**
     * Set the time string, if the provided time is not null
     *
     * @param time A LocalTime representing the time to set
     */
    public void setValue(LocalTime time) {
        if (time != null)
            setText(time.format(FORMATTER));
    }

}
