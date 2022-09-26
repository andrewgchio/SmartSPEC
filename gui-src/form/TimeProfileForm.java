package form;

import javafx.event.ActionEvent;
import javafx.scene.Node;
import javafx.scene.control.CheckBox;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import model.Model;
import model.TimeProfilesModel;
import model.TimeProfilesModel.TimeProfile;
import view.View;
import viewutils.DateField;
import viewutils.IntField;
import viewutils.TimeField;

public class TimeProfileForm extends Form {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Instance Variables
    ////////////////////////////////////////////////////////////////////////////

    protected TextField name;

    protected HBox startDT;
    protected HBox endDT;
    protected HBox reqTime;

    protected VBox recurrence;
    
    private static final int DETAILS_ALLOC_SPACE = 40;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Construct a Time Profile Entry Form
     */
    public TimeProfileForm() {
        super();

        // Name for pattern
        name = new TextField();
        addEntry("Name/Description: ", name);

        // Options for pattern
        addEntry("DateTime");

        startDT = makeDateTimeEntry();
        addSubEntry("Start Date/Time: ", startDT);

        endDT = makeDateTimeEntry();
        addSubEntry("End Date/Time:", endDT);
        
        reqTime = makeTimeEntry();
        addSubEntry("Required Time:", reqTime);

        // Options for recurrence
        addEntry("Recurrence");

        recurrence = makeRecurrenceEntry();
        addSubEntry("Repeat every:", recurrence);

        addSave(this::saveForm);

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Form Construction Helpers
    ////////////////////////////////////////////////////////////////////////////
    
    private HBox makeTimeEntry() {
        return new HBox(new TimeField(), new Label("+/-"), new TimeField());
    }

    private HBox makeDateTimeEntry() {
        return new HBox(
                new DateField(), 
                new Label(" at "), 
                new TimeField(), 
                new Label("+/-"), 
                new TimeField());
    }

    private VBox makeRecurrenceEntry() {
        VBox recurrence = new VBox();

        // n
        IntField n = new IntField();

        // period
        ChoiceBox<String> period = new ChoiceBox<>();
        period.getItems().addAll("day", "week");

        // details
        VBox details = new VBox();
        details.setMinHeight(DETAILS_ALLOC_SPACE);

        // change contents of details based on choice
        period.setOnAction(e -> {
            switch (period.getValue()) {
            case "day":
                details.getChildren().clear();
                break;
            case "week":
                HBox days = new HBox();
                days.setSpacing(SPACING);
                days.getChildren().addAll(
                        new VBox(new Label("Su"), new CheckBox()),
                        new VBox(new Label("M"),  new CheckBox()), 
                        new VBox(new Label("Tu"), new CheckBox()),
                        new VBox(new Label("W"),  new CheckBox()), 
                        new VBox(new Label("Th"), new CheckBox()),
                        new VBox(new Label("F"),  new CheckBox()), 
                        new VBox(new Label("Sa"), new CheckBox()));
                details.getChildren().add(days);
                break;
            }
        });

        recurrence.getChildren().addAll(new HBox(n, period), details);
        return recurrence;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // I/O Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Save the input provided by the user into a time profile
     * 
     * @param e
     */
    @SuppressWarnings("unchecked")
    private void saveForm(ActionEvent e) {
        TimeProfilesModel tpm = Model.getTimeProfiles();
        TimeProfile       tp  = tpm.new TimeProfile();

        // name
        tp.setName(name.getText());

        // start datetime
        tp.setStartDate(((DateField) startDT.getChildren().get(0)).getValue());
        tp.setStartTime(((TimeField) startDT.getChildren().get(2)).getValue());
        tp.setStartStdev(((TimeField) startDT.getChildren().get(4)).getValue());

        // end datetime
        tp.setEndDate(((DateField) endDT.getChildren().get(0)).getValue());
        tp.setEndTime(((TimeField) endDT.getChildren().get(2)).getValue());
        tp.setEndStdev(((TimeField) endDT.getChildren().get(4)).getValue());
        
        // req time
        tp.setReqTime(((TimeField) reqTime.getChildren().get(0)).getValue());
        tp.setReqStdev(((TimeField) reqTime.getChildren().get(2)).getValue());

        // repeat
        HBox rep = ((HBox) recurrence.getChildren().get(0));
        tp.setRepeat(((IntField) rep.getChildren().get(0)).getValue());
        tp.setPeriod(((ChoiceBox<String>) rep.getChildren().get(1)).getValue());

        // repeat days
        if (tp.getPeriod().equals("week")) {
            VBox details = (VBox) recurrence.getChildren().get(1);
            HBox days    = (HBox) details.getChildren().get(0);
            for (Node n : days.getChildren()) {
                VBox dayEnt = (VBox) n;
                String   day = ((Label) dayEnt.getChildren().get(0)).getText();
                CheckBox cb  = (CheckBox) dayEnt.getChildren().get(1);
                if (cb.isSelected())
                    tp.addDay(day);
            }
        }

        tpm.addTimeProfile(tp);

        View.log("Saved time profile " + tp.getDisplayName());
    }

}
