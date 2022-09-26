package viewutils;

import java.util.Iterator;
import java.util.function.Supplier;

import javafx.scene.control.Button;
import javafx.scene.control.ScrollPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;

public class ScrollVBox extends VBox implements Iterable<HBox> {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Instance Variables
    ////////////////////////////////////////////////////////////////////////////

    private VBox           selected;
    private Supplier<HBox> f;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constants
    ////////////////////////////////////////////////////////////////////////////

    public static final int SPACING     = 3;
    public static final int PANE_WIDTH  = 500;
    public static final int PANE_HEIGHT = 200;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Construct a ScrollPaneEntry object
     * 
     * @param label The button label
     * @param f     A function to produce data entry fields to add
     */
    public ScrollVBox(Supplier<HBox> f) {

        this.f = f;

        // ScrollPane of VBox selected entries
        ScrollPane scroll = new ScrollPane();
        scroll.setMinWidth(PANE_WIDTH);
        scroll.setMinHeight(PANE_HEIGHT);

        // selected entries
        selected = new VBox();
        selected.setSpacing(SPACING);
        scroll.setContent(selected);

        // add button
        Button addButton = new Button("Add new...");
        addButton.setOnAction(e -> selected.getChildren().add(newEntry()));

        this.getChildren().addAll(scroll, addButton);
    }
    
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Iterators
    ////////////////////////////////////////////////////////////////////////////
    
    @Override
    public Iterator<HBox> iterator() {
        return selected.getChildren().stream().map(x -> (HBox) x).iterator();
    }
    

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Queries / Access Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return a new HBox representing an entry in the ScrollPane
     * 
     * @return A new HBox representing an entry in the ScrollPane
     */
    public HBox newEntry() {
        HBox entry = f.get();
        entry.setSpacing(SPACING);

        // remove
        Button remove = new Button("-");
        remove.setOnAction(e -> selected.getChildren().remove(entry));
        entry.getChildren().add(remove);


        return entry;
    }
    
    /**
     * Set the supplier for creating new data entries
     * @param f The supplier function
     */
    public void setSupplier(Supplier<HBox> f) {
        this.f = f;
    }

    /**
     * Removes all entries
     */
    public void clear() {
        selected.getChildren().clear();
    }

    /**
     * Add the data entry
     * 
     * @param data The data entry
     */
    public void add(HBox data) {
        selected.getChildren().add(data);
    }
    
    /**
     * Remove the add button if it exists
     */
    public void removeAddButton() {
        if (this.getChildren().size() == 2) 
            this.getChildren().remove(1);
    }

}
