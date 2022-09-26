package model;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import model.MetaEventsModel.MetaEvent;
import view.View;

public class EventsModel extends EntityModel<EventsModel.Event> {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Singleton Construction
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Singleton instance of EventsModel class
     */
    private static final EventsModel instance = new EventsModel();

    /**
     * Return an instance of the EventsModel class
     * 
     * @return An instance of the EventsModel class
     */
    public static EventsModel getInstance() { return instance; }

    /**
     * Private default constructor for singleton
     */
    private EventsModel() {}

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Event Class Definition
    ////////////////////////////////////////////////////////////////////////////

    public class Event extends EntityModel<EventsModel.Event>.Entity {

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Instance Variables
        ////////////////////////////////////////////////////////////////////////

        private Integer               metaid   = null;
        private String                tp       = null;
        private Set<Integer>          spaceIDs = new HashSet<>();
        private Map<Integer, Integer> capacity = new HashMap<>();

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Getter Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Return the metaevent ID
         * 
         * @return The metaevent ID
         */
        public int getMetaID() {
            return this.metaid;
        }

        /**
         * Return the name of the time profile
         * 
         * @return The name of the time profile
         */
        public String getProfileName() {
            return this.tp;
        }

        /**
         * Return the set of host spaces
         * 
         * @return The set of host spaces
         */
        public Set<Integer> getSpaceIDs() {
            return this.spaceIDs;
        }

        /**
         * Return the attendance capacities
         * 
         * @return The attendance capacities
         */
        public Map<Integer, Integer> getCapacity() {
            return capacity;
        }

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Setter Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Set the metaevent ID
         * 
         * @param id The metaevent ID
         */
        public void setMetaID(Integer id) {
            this.metaid = id;
        }

        /**
         * Set the name of the time profile
         * 
         * @param name The name of the time profile
         */
        public void setProfile(String name) {
            this.tp = name;
        }

        /**
         * Add a space ID to the set of hosting spaces
         * 
         * @param id The space to add
         */
        public void addSpaceID(Integer id) {
            this.spaceIDs.add(id);
        }

        /**
         * Add an attendance capacity
         * 
         * @param mpid The metaperson ID
         * @param cap  The capacity
         */
        public void addCapacity(Integer mpid, Integer cap) {
            this.capacity.put(mpid, cap);
        }

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Other Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Return true if the event is valid
         * 
         * @return True if the event is valid
         */
        public boolean isValid() {
            return id != null && metaid != null && tp != null 
                    && !spaceIDs.isEmpty() && !capacity.isEmpty();
        }

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Queries / Access Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Remove an event from the model
     * 
     * @param e The event to remove
     */
    public void remove(Event e) {
        idMap.remove(e.id);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // I/O Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Load the provided events file
     * 
     * @param f The file to load
     */
    public void loadFile(File f) {
        if (f == null) {
            View.log("Error: No events file selected");
            return;
        }

        try {
            JSONParser parser = new JSONParser();

            JSONArray arr = (JSONArray) parser.parse(new FileReader(f));
            for (Object element : arr) {
                JSONObject o = (JSONObject) element;

                Event e = new Event();

                // id
                e.setID((int) (long) o.get("id"));

                // metaevent-id
                e.setMetaID((int) (long) o.get("metaevent-id"));

                // description
                e.setDescription((String) o.get("description"));

                // profile-index
                MetaEvent me = Model.getMetaEvents().get(e.metaid);
                e.setProfile(me.getProfileName(
                        (int) (long) o.get("profile-index")));

                // space-ids
                JSONArray spaceIDs = (JSONArray) o.get("space-ids");
                for (Object spaceID : spaceIDs)
                    e.addSpaceID((int) (long) spaceID);

                // capacity
                JSONArray capacity = (JSONArray) o.get("capacity");
                for (Object element2 : capacity) {
                    JSONObject capEntry = (JSONObject) element2;
                    JSONArray  capRange = (JSONArray) capEntry.get("range");
                    e.addCapacity((int) (long) capEntry.get("metaperson-id"), (int) (long) capRange.get(1));
                }

                this.add(e);
            }

            View.log("Loaded Events File: " + f.getAbsolutePath());

        } 
        catch (IllegalArgumentException e) {
            View.log("Error: " + e.getMessage());
        } 
        catch (ParseException e) {
            View.log("Error: Cannot parse file: " + f.getAbsolutePath());
        } 
        catch (IOException e) {
            View.log("Error: Invalid events file: " + f.getAbsolutePath());
        }
    }

    /**
     * Write the events model to file
     */
    @SuppressWarnings("unchecked")
    public void writeToFile() {
        if (!checkValid(true))
            return;

        String fname = Model.getConfig().getEventsPath();

        JSONArray data = new JSONArray();

        for (Event e : this) {
            JSONObject o = new JSONObject();

            // id
            o.put("id", e.getID());

            // metaevent-id
            o.put("metaevent-id", e.getMetaID());

            // description
            o.put("description", e.getDescription());

            // profile-index
            MetaEvent me = Model.getMetaEvents().get(e.id);
            o.put("profile-index", me.getProfileIndex(e.getProfileName()));

            // space-ids
            JSONArray spaceIDs = new JSONArray();
            for (Integer x : e.getSpaceIDs())
                spaceIDs.add(x);
            o.put("space-ids", spaceIDs);

            // capacity
            JSONArray capacity = new JSONArray();
            for (Entry<Integer, Integer> cap : e.getCapacity().entrySet()) {
                JSONObject capEntry = new JSONObject();
                capEntry.put("metaperson-id", cap.getKey());

                JSONArray range = new JSONArray();
                range.add(0);
                range.add(cap.getValue());
                capEntry.put("range", range);
                capacity.add(capEntry);
            }

            data.add(o);
        }

        Model.getInstance().writeJSON(fname, data, "events");

    }

}
