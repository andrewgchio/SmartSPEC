package viewutils;

import java.util.AbstractMap.SimpleEntry;
import java.util.function.Predicate;
import java.util.stream.Stream;

import javafx.scene.control.ChoiceBox;
import model.EntityModel;

/**
 * A ChoiceBox that provides default display names from a stream
 */
public class IDChoiceBox extends ChoiceBox<String> {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Construct an IDChoiceBox from the given stream
     * 
     * @param stream A stream of Entity objects
     */
    public IDChoiceBox(Stream<? extends EntityModel<?>.Entity> stream) {
        super();

        stream.sorted((x, y) -> Integer.compare(x.getID(), y.getID()))
              .map(x -> x.getDisplayName())
              .forEach(x -> this.getItems().add(x));

    }

    /**
     * Construct an IDChoiceBox from the given (filtered) stream
     * 
     * @param stream
     * @param p
     */
    public IDChoiceBox(Stream<? extends EntityModel<?>.Entity> stream,
            Predicate<? super EntityModel<?>.Entity> predicate) {
        this(stream.filter(x -> predicate.test(x)));
    }
    
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Getter Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return the entity ID that was selected
     * 
     * @return The entity ID that was selected
     */
    public Integer getSelectedID() {
        String id = this.extractID(this.getValue());
        if (id == null) 
            return null;
        else
            return Integer.parseInt(this.extractID(this.getValue()));
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Setter Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Set the value to the entry with the given ID
     * 
     * @param id The ID to set to
     */
    public void setSelectedID(Integer id) {
        this.getItems().stream()
                .map(x -> new SimpleEntry<>(Integer.parseInt(extractID(x)), x))
                .filter(x -> id.equals(x.getKey()))
                .findFirst()
                .ifPresent(x -> this.setValue(x.getValue()));
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Other Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Extract the ID from a display name
     * 
     * @param value A display name
     * @return The ID from a display name
     */
    private String extractID(String value) {
        if (value.isBlank()) 
            return null;
        return value.substring(0, value.indexOf("(") - 1);
    }

}
