package form;

import java.util.Map.Entry;
import java.util.function.Supplier;

import javafx.event.ActionEvent;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.HBox;
import model.EventsModel;
import model.EventsModel.Event;
import model.MetaEventsModel;
import model.MetaEventsModel.MetaEvent;
import model.MetaPeopleModel;
import model.Model;
import model.SpacesModel.Space;
import view.View;
import viewutils.IDChoiceBox;
import viewutils.IDComboBox;
import viewutils.IntField;
import viewutils.ScrollVBox;
import viewutils.TPChoiceBox;

public class EventForm extends Form {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Instance Variables
    ////////////////////////////////////////////////////////////////////////////

    protected IDComboBox  id;
    protected IDChoiceBox metaid;
    protected TextField   description;
    protected TPChoiceBox tp;
    protected Label       space;
    protected ScrollVBox  capacity;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Construct an Event Form based on a space
     * 
     * @param c The hosting space
     */
    public EventForm(Space c) {
        super();

        // id
        id = makeIDEntry(c);
        addEntry("Event ID:", id);

        // metaevent-id
        metaid = makeMetaEventEntry();
        addEntry("MetaEvent ID:", metaid);

        // description
        description = new TextField();
        addEntry("Description", description);

        // space-ids
        space = new Label(c.getID().toString());
        addEntry("Spaces:", space);

        // time profile
        tp = new TPChoiceBox();
        addEntry("Time Profile:", tp);

        // capacity
        capacity = new ScrollVBox(null);
        capacity.removeAddButton();
        addEntry("Capacity:", capacity);

        addSave(this::saveForm);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Form Construction Helpers
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Create an ID entry box that modifies all other entries when selected
     * 
     * @param c The hosting space
     * @return An ID entry box that modifies all other entries when selected
     */
    private IDComboBox makeIDEntry(Space c) {
        EventsModel em = Model.getEvents();

        IDComboBox entry = new IDComboBox(em.stream(), 
                x -> ((Event) x).getSpaceIDs().contains(c.getID()));

        entry.setOnAction(e2 -> {
            Event e = em.get(entry.getSelectedID());

            // If event exists, then set all other data entries
            if (e != null) {
                MetaEvent me = Model.getMetaEvents().get(e.getMetaID());

                // metaid
                metaid.setSelectedID(e.getMetaID());

                // description
                description.setText(e.getDescription());

                // space - does not change

                // time profile
                tp.setOptions(me);
                tp.setTimeProfile(e.getProfileName());

                // capacity
                capacity.clear();
                capacity.setSupplier(makeSelectCapEntry(me));
                for (Entry<Integer, Integer> ent : e.getCapacity().entrySet()) {
                    HBox cap = capacity.newEntry();

                    // choice
                    IDChoiceBox choice = (IDChoiceBox) cap.getChildren().get(0);
                    choice.setSelectedID(ent.getKey());

                    // hi mean
                    IntField hi = (IntField) cap.getChildren().get(1);
                    hi.setValue(ent.getValue());

                    capacity.add(cap);
                }

            }

        });

        return entry;
    }

    /**
     * Create an ID entry box that further modifies other entries when selected
     * 
     * @return A metaevent ID ChoiceBox
     */
    private IDChoiceBox makeMetaEventEntry() {
        MetaEventsModel mem = Model.getMetaEvents();

        IDChoiceBox entry = new IDChoiceBox(mem.stream());

        entry.setOnAction(e -> {
            MetaEvent me = mem.get(entry.getSelectedID());

            // description
            description.setText(me.getDescription());

            // space - does not change

            // time profile
            tp.setOptions(me);

            // capacity
            capacity.clear();
            capacity.setSupplier(makeSelectCapEntry(me));
            for (Integer mpid : me.getCapacity().keySet()) {
                HBox cap = capacity.newEntry();

                // choice
                IDChoiceBox choice = (IDChoiceBox) cap.getChildren().get(0);
                choice.setSelectedID(mpid);

                // clear hi

                capacity.add(cap);
            }

        });

        return entry;
    }

    /**
     * Create the cap entry options for the given metaevent
     * 
     * @param me The metaevent
     * @return The cap entry options for the given metaevent
     */
    private Supplier<HBox> makeSelectCapEntry(MetaEvent me) {
        MetaPeopleModel mpm = Model.getMetaPeople();
        return () -> new HBox(
                new IDChoiceBox(me.getCapacity().keySet().stream()
                        .map(x -> mpm.get(x)).filter(x -> true)),
                new IntField());
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // I/O Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Save the input provided by the user into an event
     * 
     * @param ae
     */
    private void saveForm(ActionEvent ae) {
        EventsModel em = Model.getEvents();
        Event       e  = em.new Event();

        // id
        e.setID(id.getSelectedID());

        // metaid
        e.setMetaID(metaid.getSelectedID());

        // description
        e.setDescription(description.getText());

        // time profile
        e.setProfile(tp.getSelectedProfile());

        // space ids
        e.addSpaceID(Integer.parseInt(space.getText()));

        // capacity
        for (HBox entry : capacity) {
            IDChoiceBox cap = (IDChoiceBox) entry.getChildren().get(0);
            Integer     id  = cap.getSelectedID();

            if (id != null) {
                Integer hi = ((IntField) entry.getChildren().get(1)).getValue();
                e.addCapacity(id, hi);
            }
        }

        em.add(e);

        View.log("Saved event " + e.getDisplayName());

    }

}
