package form;

import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.scene.Node;
import javafx.scene.control.Button;
import javafx.scene.control.ScrollPane;
import javafx.scene.control.TextField;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import model.MetaEventsModel;
import model.MetaEventsModel.MetaEvent;
import model.Model;
import view.View;
import viewutils.DoubleField;
import viewutils.IntField;

public class MetaEventsDeclareForm extends Form {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Instance Variables
    ////////////////////////////////////////////////////////////////////////////

    protected VBox declare;

    public static final int DECLARE_WIDTH  = 300;
    public static final int DECLARE_HEIGHT = 200;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Construct a MetaEvents Declaration Form
     */
    public MetaEventsDeclareForm() {
        super();

        // declarations
        declare = makeMetaEventsDeclareEntry();
        addEntry("Declare: ", declare);

        addSave(this::saveForm);

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Form Constructors Helpers
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Create the node to declare metaevents
     * 
     * @return The node to declare metaevents
     */
    private VBox makeMetaEventsDeclareEntry() {
        VBox declareEntries = new VBox();

        // ScrollPane of VBox entries
        ScrollPane scroll = new ScrollPane();
        scroll.setMinHeight(DECLARE_HEIGHT);
        scroll.setMinWidth(DECLARE_WIDTH);
        VBox entries = new VBox();
        entries.setSpacing(SPACING);
        scroll.setContent(entries);

        // Button to add new entries
        Button addButton = new Button("Add new");
        addButton.setOnAction(e -> {
            HBox entry = makeDeclareEntry(entries);
            entries.getChildren().add(entry);
        });

        // "Click addButton once
        addButton.fire();

        declareEntries.getChildren().addAll(scroll, addButton);
        return declareEntries;
    }

    /**
     * Create a declaration entry
     * 
     * @param entries The node to add this object
     * @return A declaration entry
     */
    private HBox makeDeclareEntry(VBox entries) {
        HBox entry = new HBox();
        entry.setSpacing(SPACING);

        // id
        IntField id = new IntField();

        // description
        TextField description = new TextField();

        // probability
        DoubleField pr = new DoubleField();

        // remove
        Button remove = new Button("-");
        remove.setOnAction(e -> entries.getChildren().remove(entry));

        entry.getChildren().addAll(id, description, pr, remove);
        return entry;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // I/O Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Save the input provided by the user into metaevents
     * 
     * @param e
     */
    private void saveForm(ActionEvent e) {
        MetaEventsModel mem = Model.getMetaEvents();

        ScrollPane scroll = (ScrollPane) declare.getChildren().get(0);
        for (Node n : ((VBox) scroll.getContent()).getChildren()) {
            ObservableList<Node> entry = ((HBox) n).getChildren();

            // id
            Integer id = ((IntField) entry.get(0)).getValue();
            if (mem.containsID(id)) {
                View.log("Duplicate metaevent id " + id + " (ignored)");
                continue;
            }

            // description
            String description = ((TextField) entry.get(1)).getText();

            // probability
            Double pr = ((DoubleField) entry.get(2)).getValue();

            // Create metaevent object
            MetaEvent me = mem.new MetaEvent();
            me.setID(id);
            me.setDescription(description);
            me.setProbability(pr);
            mem.add(me);

            View.log("Declared metaevent " + me.getDisplayName());
        }

    }

}