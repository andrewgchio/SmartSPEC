package viewutils;

import java.util.AbstractMap.SimpleEntry;
import java.util.function.Predicate;
import java.util.stream.Stream;

import javafx.scene.control.ComboBox;
import model.EntityModel;

/**
 * A ComboBox that provides default display names from a stream with edit
 * permissions.
 */
public class IDComboBox extends ComboBox<String> {
    
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////
    
    /**
     * Construct an IDComboBox from the given stream
     * 
     * @param stream A stream of Entity objects
     */
    public IDComboBox(Stream<? extends EntityModel<?>.Entity> stream) {
        super();
        
        stream.sorted( (x,y) -> Integer.compare(x.getID(), y.getID()))
              .map(x -> x.getDisplayName())
              .forEach(x -> this.getItems().add(x));
        
        this.setEditable(true);
    }
    
    /**
     * Construct an IDComboBox from the given (filtered) stream
     * 
     * @param stream
     * @param p
     */
    public IDComboBox(Stream<? extends EntityModel<?>.Entity> stream,
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
        int idx = value.indexOf("(");
        if (value.isBlank())
            return null;
        else if (idx == -1)
            return value.strip();
        else
            return value.substring(0, value.indexOf("(") - 1);
    }

}
