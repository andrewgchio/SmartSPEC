package form;

import java.util.HashSet;
import java.util.List;
import java.util.Map.Entry;
import java.util.Set;
import java.util.stream.Collectors;

import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.scene.Node;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import model.MetaEventsModel;
import model.MetaEventsModel.MetaEvent;
import model.Model;
import model.TimeProfilesModel.TimeProfile;
import view.View;
import viewutils.DoubleField;
import viewutils.IDChoiceBox;
import viewutils.IntField;
import viewutils.ScrollVBox;
import viewutils.TPChoiceBox;

public class MetaEventForm extends Form {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Instance Variables
    ////////////////////////////////////////////////////////////////////////////

    protected IDChoiceBox id;
    protected TextField   description;
    protected DoubleField probability;
    protected VBox        spaces;
    protected ScrollVBox  profiles;
    protected ScrollVBox  capacity;

    private Set<Integer> selectedSpaces = new HashSet<>();

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Construct a MetaEvent Form
     */
    public MetaEventForm() {
        super();

        // id
        id = makeIDEntry();
        addEntry("MetaEvent ID:", id);

        // description
        description = new TextField();
        addEntry("Description:", description);

        // probability
        probability = new DoubleField();
        addEntry("Probability:", probability);

        // spaces
        spaces = makeSpacesEntry();
        addEntry("Spaces:", spaces);

        // time profile
        profiles = new ScrollVBox(this::makeSelectProfileEntry);
        addEntry("Time Profiles:", profiles);

        // capacity
        capacity = new ScrollVBox(this::makeSelectCapEntry);
        addEntry("Capacity :", capacity);

        addSave(this::saveForm);

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Form Construction Helpers
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Create an ID entry box that modifies all other entries when selected
     * 
     * @return A metaevent ID ChoiceBox
     */
    private IDChoiceBox makeIDEntry() {
        MetaEventsModel mem = Model.getMetaEvents();

        IDChoiceBox entry = new IDChoiceBox(mem.stream());

        entry.setOnAction(e -> {
            MetaEvent me = mem.get(entry.getSelectedID());

            // description
            description.setText(me.getDescription());

            // probability
            probability.setValue(me.getProbability());

            // spaces
            selectedSpaces.clear();
            selectedSpaces.addAll(me.getSpaceIDs());
            updateSpacesLabel((Label) spaces.getChildren().get(0));

            // time profiles
            profiles.clear();
            for (Entry<Double, TimeProfile> profile : me.getTimeProfiles()) {
                HBox tp = profiles.newEntry();

                // choice
                TPChoiceBox choice = (TPChoiceBox) tp.getChildren().get(0);
                choice.setTimeProfile(profile.getValue().getName());

                // probability
                DoubleField pr = (DoubleField) tp.getChildren().get(1);
                pr.setValue(profile.getKey());

                profiles.add(tp);
            }

            // capacity
            capacity.clear();
            for (Entry<Integer, List<Integer>> ent : 
                    me.getCapacity().entrySet()) {
                HBox cap = capacity.newEntry();

                // choice
                IDChoiceBox choice = (IDChoiceBox) cap.getChildren().get(0);
                choice.setSelectedID(ent.getKey());

                // hi mean
                IntField hi_m = (IntField) cap.getChildren().get(1);
                hi_m.setValue(ent.getValue().get(2));

                // hi stdev
                IntField hi_s = (IntField) cap.getChildren().get(3);
                hi_s.setValue(ent.getValue().get(3));

                capacity.add(cap);
            }

        });

        return entry;
    }

    /**
     * Create the data entry for spaces
     * 
     * @return The data entry for spaces
     */
    private VBox makeSpacesEntry() {
        // selected items
        Label selected = new Label("[]");

        // choices
        IDChoiceBox choice = new IDChoiceBox(Model.getSpaces().stream());

        Button addButton = new Button("+");
        addButton.setOnAction(e -> {
            Integer cid = choice.getSelectedID();
            selectedSpaces.add(cid);
            updateSpacesLabel(selected);
        });

        Button removeButton = new Button("-");
        removeButton.setOnAction(e -> {
            Integer cid = choice.getSelectedID();
            selectedSpaces.remove(cid);
            updateSpacesLabel(selected);
        });

        HBox selectionEntry = new HBox(choice, addButton, removeButton);
        selectionEntry.setSpacing(SPACING);

        return new VBox(selected, selectionEntry);
    }

    /**
     * Update the given label based on the selected spaces
     * 
     * @param spacesLabel The label to update
     */
    private void updateSpacesLabel(Label spacesLabel) {
        spacesLabel.setText(
                selectedSpaces.stream()
                              .sorted()
                              .map(x -> x.toString())
                              .collect(Collectors.joining(",", "[", "]")));
    }

    /**
     * Create the data entry for time profiles
     * 
     * @return The data entry for time profiles
     */
    private HBox makeSelectProfileEntry() {
        return new HBox(
                new TPChoiceBox(Model.getTimeProfiles().stream()), 
                new DoubleField());
    }

    /**
     * Create the data entry for attendance capacities
     * 
     * @return The data entry for attendance capacities
     */
    private HBox makeSelectCapEntry() {
        return new HBox(
                new IDChoiceBox(Model.getMetaPeople().stream()), 
                new IntField(), 
                new Label(" +/- "),
                new IntField());
    }
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // I/O Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Save the input provided by the user into a metaevent
     * 
     * @param e
     */
    private void saveForm(ActionEvent e) {
        MetaEventsModel mem = Model.getMetaEvents();
        MetaEvent       me  = mem.new MetaEvent();

        // id
        me.setID(Integer.parseInt(id.getValue().split(" ")[0]));

        // description
        me.setDescription(description.getText());

        // probability
        me.setProbability(probability.getValue());

        // spaces
        me.getSpaceIDs().clear();
        for (Integer n : selectedSpaces)
            me.addSpaceID(n);
        me.setNSpace(1);

        // time profiles
        for (HBox entry : profiles) {
            ObservableList<Node> data = entry.getChildren();

            TPChoiceBox tp   = (TPChoiceBox) data.get(0);
            String      name = tp.getSelectedProfile();
            if (name != null) {
                Double pr = ((DoubleField) data.get(1)).getValue();
                me.addTimeProfile(pr, Model.getTimeProfiles().get(name));
            }
        }

        // capacity
        for (HBox entry : capacity) {
            ObservableList<Node> data = entry.getChildren();

            IDChoiceBox cap = (IDChoiceBox) data.get(0);
            Integer     id  = cap.getSelectedID();
            if (id != null) {
                Integer hi_m = ((IntField) data.get(1)).getValue();
                Integer hi_s = ((IntField) data.get(3)).getValue();
                me.addCapacity(id, 0, 0, hi_m, hi_s);
            }
        }

        mem.add(me);

        View.log("Saved metaevent " + me.getDisplayName());

    }

}
