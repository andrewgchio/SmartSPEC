package viewutils;

import java.util.stream.Stream;

import javafx.scene.control.ChoiceBox;
import model.MetaEventsModel.MetaEvent;
import model.MetaPeopleModel.MetaPerson;
import model.Model;
import model.TimeProfilesModel;

/**
 * A ChoiceBox that provides display names for time profiles
 */
public class TPChoiceBox extends ChoiceBox<String> {
    
    //////////////////////////////////////////////////////////////////////////// 
    //////////////////////////////////////////////////////////////////////////// 
    // Constructors
    //////////////////////////////////////////////////////////////////////////// 
    
    /**
     * Construct a TimeProfile ChoiceBox with no options
     */
    public TPChoiceBox() {
        super();
    }
    
    /**
     * Construct a TimeProfile ChoiceBox from the given stream
     * @param stream A stream of Time Profiles
     */
    public TPChoiceBox(Stream<TimeProfilesModel.TimeProfile> stream) {
        super();
        
        stream.sorted( (x, y) -> x.getName().compareTo(y.getName()))
              .map(x -> x.getDisplayName())
              .forEach(x -> this.getItems().add(x));
    }
    
    /**
     * Construct a TimeProfile ChoiceBox from the metaperson's time profiles
     * @param mp A metaperson
     */
    public TPChoiceBox(MetaPerson mp) {
        this(mp.getTimeProfiles().stream().map(x -> x.getValue()));
    }
    
    /**
     * Construct a TimeProfile ChoiceBox from the metaevent's time profiles
     * @param me A metaevent
     */
    public TPChoiceBox(MetaEvent me) {
        this(me.getTimeProfiles().stream().map(x -> x.getValue()));
    }
    
    //////////////////////////////////////////////////////////////////////////// 
    //////////////////////////////////////////////////////////////////////////// 
    // Getter Methods
    //////////////////////////////////////////////////////////////////////////// 
    
    /**
     * Return the name of the selected time profile
     * @return The name of the selected time profile
     */
    public String getSelectedProfile() {
        return getValue().substring(0, getValue().indexOf("(") - 1);
    }
    
    //////////////////////////////////////////////////////////////////////////// 
    //////////////////////////////////////////////////////////////////////////// 
    // Setter Methods
    //////////////////////////////////////////////////////////////////////////// 
    
    /**
     * Set the value to the time profile with the given name
     * @param name The name of the time profile
     */
    public void setTimeProfile(String name) {
        this.setValue(Model.getTimeProfiles().get(name).getDisplayName());
    }
    
    /**
     * Set the options to the given stream
     * @param stream The stream of time profiles
     */
    public void setOptions(Stream<TimeProfilesModel.TimeProfile> stream) {
        this.getItems().clear();
        stream.sorted( (x, y) -> x.getName().compareTo(y.getName()))
              .map(x -> x.getDisplayName())
              .forEach(x -> this.getItems().add(x));
    }
    
    /**
     * Set the options from the metaperson's time profiles
     * @param mp The metaperson
     */
    public void setOptions(MetaPerson mp) {
        setOptions(mp.getTimeProfiles().stream().map(x -> x.getValue()));
    }
    
    /**
     * Set the options from the metaevents's time profiles
     * @param me The metaevent
     */
    public void setOptions(MetaEvent me) {
        setOptions(me.getTimeProfiles().stream().map(x -> x.getValue()));
    }
    
}
