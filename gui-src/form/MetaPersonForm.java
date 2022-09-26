package form;

import java.util.Map.Entry;

import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.scene.Node;
import javafx.scene.control.TextField;
import javafx.scene.layout.HBox;
import model.MetaPeopleModel;
import model.MetaPeopleModel.MetaPerson;
import model.Model;
import model.TimeProfilesModel.TimeProfile;
import view.View;
import viewutils.DoubleField;
import viewutils.IDChoiceBox;
import viewutils.ScrollVBox;
import viewutils.TPChoiceBox;

public class MetaPersonForm extends Form {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Instance Variables
    ////////////////////////////////////////////////////////////////////////////

    protected IDChoiceBox id;
    protected TextField   description;
    protected DoubleField probability;
    protected ScrollVBox  profiles;
    protected ScrollVBox  affinity;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Construct a MetaPerson Form
     */
    public MetaPersonForm() {
        super();

        // id
        id = makeIDEntry();
        addEntry("MetaPerson ID:", id);

        // description
        description = new TextField();
        addEntry("Description:", description);

        // probability
        probability = new DoubleField();
        addEntry("(Relative) Count:", probability);

        // time profiles
        profiles = new ScrollVBox(this::makeSelectProfileEntry);
        addEntry("Time Profiles:", profiles);

        // event affinity
        affinity = new ScrollVBox(this::makeSelectAffinityEntry);
        addEntry("Event Affinity:", affinity);

        addSave(this::saveForm);

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Form Construction Helpers
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Create an ID entry box that modifies all other entries when selected
     * 
     * @return A metapeople ID ChoiceBox
     */
    private IDChoiceBox makeIDEntry() {
        MetaPeopleModel mpm = Model.getMetaPeople();

        IDChoiceBox entry = new IDChoiceBox(mpm.stream());

        entry.setOnAction(e -> {
            MetaPerson mp = mpm.get(entry.getSelectedID());

            // description
            description.setText(mp.getDescription());

            // probability
            probability.setValue(mp.getProbability());

            // time profiles
            profiles.clear();
            for (Entry<Double, TimeProfile> profile : mp.getTimeProfiles()) {
                HBox tp = profiles.newEntry();

                // choice
                TPChoiceBox choice = (TPChoiceBox) tp.getChildren().get(0);
                choice.setTimeProfile(profile.getValue().getName());

                // probability
                DoubleField pr = (DoubleField) tp.getChildren().get(1);
                pr.setValue(profile.getKey());

                profiles.add(tp);
            }

            // event affinity
            affinity.clear();
            for (Entry<Integer, Double> e2 : mp.getEventAffinity().entrySet()) {
                HBox aff = affinity.newEntry();

                // choice
                IDChoiceBox choice = (IDChoiceBox) aff.getChildren().get(0);
                choice.setSelectedID(e2.getKey());

                // probability
                DoubleField pr = (DoubleField) aff.getChildren().get(1);
                pr.setValue(e2.getValue());

                affinity.add(aff);
            }

        });

        return entry;
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
     * Create the data entry for event affinities
     * 
     * @return The data entry for event affinities
     */
    private HBox makeSelectAffinityEntry() {
        return new HBox(
                new IDChoiceBox(Model.getMetaEvents().stream()), 
                new DoubleField());
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // I/O Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Save the input provided by the user into a metaperson
     * 
     * @param e
     */
    private void saveForm(ActionEvent e) {
        MetaPeopleModel mpm = Model.getMetaPeople();
        MetaPerson      mp  = mpm.new MetaPerson();

        // id
        mp.setID(Integer.parseInt(id.getValue().split(" ")[0]));

        // description
        mp.setDescription(description.getText());

        // probability
        mp.setProbability(probability.getValue());

        // time profiles
        for (HBox entry : profiles) {
            ObservableList<Node> data = entry.getChildren();

            TPChoiceBox tp   = (TPChoiceBox) data.get(0);
            String      name = tp.getSelectedProfile();
            if (name != null) {
                Double pr = ((DoubleField) data.get(1)).getValue();
                mp.addTimeProfile(pr, Model.getTimeProfiles().get(name));
            }
        }


        // event affinity
        for (HBox entry : affinity) {
            ObservableList<Node> data = entry.getChildren();

            IDChoiceBox ea = (IDChoiceBox) data.get(0);
            Integer     id = ea.getSelectedID();
            if (id != null) {
                Double pr = ((DoubleField) data.get(1)).getValue();
                mp.addEventAffinity(id, pr);
            }
        }

        mpm.add(mp);

        View.log("Saved metaperson " + mp.getDisplayName());

    }

}
