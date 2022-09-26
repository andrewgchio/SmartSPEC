package viewutils;

import java.time.LocalDate;
import java.time.format.DateTimeFormatter;

import javafx.scene.control.TextField;

/**
 * A text field that only accepts a date of format yyyy-MM-dd
 */
public class DateField extends TextField {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constants
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Width of DateField text box
     */
    public static final int DATE_WIDTH = 100;

    /**
     * Output date format as string "yyyy-MM-dd"
     */
    public final static String FORMAT = "yyyy-MM-dd";

    /**
     * Formatter that follows the pattern "yyyy-MM-dd"
     */
    public final static DateTimeFormatter FORMATTER =
            DateTimeFormatter.ofPattern(FORMAT);

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Construct a DateField object
     */
    public DateField() {
        super();

        this.setMaxWidth(DATE_WIDTH);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Getter Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return the input date as a string
     *
     * @return A string representing the input date
     */
    public String getValue() {
        return LocalDate.parse(getText(), FORMATTER).format(FORMATTER);
    }

    /**
     * Return the input date as a LocalDate object
     *
     * @return A LocalDate object representing the input date
     */
    public LocalDate getDateValue() {
        return LocalDate.parse(getText(), FORMATTER);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Setter Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Set the date string
     *
     * @param text A string representing the date to set
     */
    public void setValue(String text) {
        setText(LocalDate.parse(text, FORMATTER).format(FORMATTER));
    }

    /**
     * Set the date string
     *
     * @param date A LocalDate object representing the date to set
     */
    public void setValue(LocalDate date) {
        if (date != null)
            setText(date.format(FORMATTER));
    }

}
