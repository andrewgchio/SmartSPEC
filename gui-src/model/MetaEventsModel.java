package model;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.AbstractMap.SimpleEntry;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import model.TimeProfilesModel.TimeProfile;
import view.View;

public class MetaEventsModel extends EntityModel<MetaEventsModel.MetaEvent> {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Singleton Construction
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Singleton instance of MetaEventsModel class
     */
    private static final MetaEventsModel instance = new MetaEventsModel();

    /**
     * Return an instance of the MetaEventsModel class
     * 
     * @return An instance of the MetaEventsModel class
     */
    public static MetaEventsModel getInstance() { return instance; }

    /**
     * Private default constructor for singleton
     */
    private MetaEventsModel() {}

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // MetaEvent Class Definition
    ////////////////////////////////////////////////////////////////////////////

    public class MetaEvent 
    extends EntityModel<MetaEventsModel.MetaEvent>.Entity {

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Instance Variables
        ////////////////////////////////////////////////////////////////////////

        private Double                           pr       = null;
        private Set<Integer>                     spaceIDs = new HashSet<>();
        private int                              nSpace   = 1;
        private List<Entry<Double, TimeProfile>> profiles = new ArrayList<>();
        private Map<Integer, List<Integer>>      capacity = new HashMap<>();

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Getter Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Return the probability of the metaevent
         * 
         * @return The probability of the metaevent
         */
        public Double getProbability() {
            return this.pr;
        }

        /**
         * Return the set of possible host spaces
         * 
         * @return The set of possible host spaces
         */
        public Set<Integer> getSpaceIDs() {
            return this.spaceIDs;
        }

        /**
         * Return the number of host spaces
         * 
         * @return The number of host spaces
         */
        public int getNSpaces() {
            return this.nSpace;
        }

        /**
         * Return the time profiles of the metaevent
         * 
         * @return The time profiles of the metaevent
         */
        public List<Entry<Double, TimeProfile>> getTimeProfiles() {
            return this.profiles;
        }

        /**
         * Return the name of the time profile at the given index
         * 
         * @param idx The index of the time profile
         * @return The name of the time profile at the given index
         */
        public String getProfileName(int idx) {
            return profiles.get(idx).getValue().getName();
        }

        /**
         * Return the index of the time profile with the given name
         * 
         * @param name The name of the time profile
         * @return The index of the time profile with the given name
         */
        public int getProfileIndex(String name) {
            for (int i = 0; i < profiles.size(); ++i) {
                if (profiles.get(i).getValue().getName().equals(name))
                    return i;
            }

            throw new IllegalArgumentException(
                    "Error: profile index with name " + name + " not found");
        }

        /**
         * Return the capacity distribution of the metaevent
         * 
         * @return The capacity distribution of the metaevent
         */
        public Map<Integer, List<Integer>> getCapacity() {
            return this.capacity;
        }

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Setter Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Set the probability of the metaevent
         * 
         * @param pr The probability
         */
        public void setProbability(Double pr) {
            this.pr = pr;
        }

        /**
         * Add a space ID to the set of host spaces
         * 
         * @param id The space ID to add
         */
        public void addSpaceID(Integer id) {
            this.spaceIDs.add(id);
        }

        /**
         * Set the number of host spaces
         * 
         * @param n The number of host spaces
         */
        public void setNSpace(Integer n) {
            this.nSpace = n;
        }

        /**
         * add a time profile with given probability
         * 
         * @param pr          The probability to add
         * @param timeProfile The time profile to add
         */
        public void addTimeProfile(Double pr, TimeProfile timeProfile) {
            profiles.add(new SimpleEntry<>(pr, timeProfile));
        }

        /**
         * Add an attendance capacity on the given metaperson
         * 
         * @param metaid The id of the metaperson
         * @param lo_m   The lower bound (mean) capacity
         * @param lo_s   The lower bound (stdev) capacity
         * @param hi_m   The upper bound (mean) capacity
         * @param hi_s   The upper bound (stdev) capacity
         */
        public void addCapacity(Integer mpid, 
                Integer lo_m, Integer lo_s, Integer hi_m, Integer hi_s) {
            List<Integer> capRanges = new ArrayList<>();
            capRanges.add(lo_m);
            capRanges.add(lo_s);
            capRanges.add(hi_m);
            capRanges.add(hi_s);
            capacity.put(mpid, capRanges);
        }

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Other Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Return true if the metaevent is valid
         * 
         * @return True if the metaevent is valid
         */
        public boolean isValid() {
            return id != null && pr != null && !spaceIDs.isEmpty() && 
                    !profiles.isEmpty() && !capacity.isEmpty();
        }

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Queries / Access Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Remove a metaevent from the model
     * 
     * @param me The metaevent to remove
     */
    public void remove(MetaEvent me) {
        // remove from metapeople
        Model.getMetaPeople().stream()
                .forEach(x -> x.getEventAffinity().remove(me.id));

        // remove metaevent
        idMap.remove(me.id);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // I/O Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Load the provided metaevents file
     * 
     * @param f The file to load
     */
    public void loadFile(File f) {
        if (f == null) {
            View.log("Error: No metaevents file selected");
            return;
        }

        try {
            JSONParser parser = new JSONParser();

            JSONArray arr = (JSONArray) parser.parse(new FileReader(f));
            for (Object element : arr) {
                JSONObject o = (JSONObject) element;

                MetaEvent me = new MetaEvent();

                // id
                me.setID((int) (long) o.get("id"));

                // description
                me.setDescription((String) o.get("description"));

                // probability
                me.setProbability((double) o.get("probability"));

                // space-ids
                JSONObject spaces = (JSONObject) o.get("spaces");
                me.setNSpace((int) (long) spaces.get("number"));
                JSONArray spaceIDs = (JSONArray) spaces.get("space-ids");
                for (Object spaceID : spaceIDs)
                    me.addSpaceID((int) (long) spaceID);

                // time-profiles
                JSONArray timeProfiles = (JSONArray) o.get("time-profiles");
                for (Object timeProfile : timeProfiles) {
                    JSONObject tpEntry = (JSONObject) timeProfile;

                    Double pr = (double) tpEntry.get("probability");

                    JSONArray   profile = (JSONArray) tpEntry.get("profile");
                    TimeProfile tp      = TimeProfilesModel.loadJSON(profile);

                    me.addTimeProfile(pr, tp);
                }

                // capacity
                JSONArray capacity = (JSONArray) o.get("capacity");
                for (Object elem : capacity) {
                    JSONObject cap = (JSONObject) elem;
                    me.addCapacity((int) (long) cap.get("metaperson-id"),
                            (int) (long) ((JSONArray) cap.get("lo")).get(0),
                            (int) (long) ((JSONArray) cap.get("lo")).get(1),
                            (int) (long) ((JSONArray) cap.get("hi")).get(0),
                            (int) (long) ((JSONArray) cap.get("hi")).get(1));
                }

                this.add(me);
            }

            View.log("Loaded MetaEvents File: " + f.getAbsolutePath());

        } 
        catch (IllegalArgumentException e) {
            View.log("Error: " + e.getMessage());
        } 
        catch (ParseException e) {
            View.log("Error: Cannot parse file: " + f.getAbsolutePath());
        } 
        catch (IOException e) {
            View.log("Error: Invalid metaevents file: " + f.getAbsolutePath());
        }
    }

    /**
     * Write the metaevents model to file
     */
    @SuppressWarnings("unchecked")
    public void writeToFile() {
        if (!checkValid(true))
            return;

        String fname = Model.getConfig().getMetaEventsPath();

        JSONArray data = new JSONArray();

        for (MetaEvent me : this) {
            JSONObject o = new JSONObject();

            // id
            o.put("id", me.id);

            // description
            o.put("description", me.description);

            // probability
            o.put("probability", me.pr);

            // spaces
            JSONObject spaces = new JSONObject();
            spaces.put("number", me.nSpace);
            JSONArray spaceIDs = new JSONArray();
            for (Integer x : me.spaceIDs)
                spaceIDs.add(x);
            spaces.put("space-ids", spaceIDs);
            o.put("spaces", spaces);

            // time-profiles
            JSONArray timeProfiles = new JSONArray();
            for (Entry<Double, TimeProfile> e : me.profiles) {
                JSONObject tpEntry = new JSONObject();
                tpEntry.put("probability", e.getKey());
                tpEntry.put("profile", 
                        TimeProfilesModel.writeJSON(e.getValue()));

                timeProfiles.add(tpEntry);
            }
            o.put("time-profiles", timeProfiles);

            // capacity
            JSONArray capacity = new JSONArray();
            for (Entry<Integer, List<Integer>> e : me.capacity.entrySet()) {
                List<Integer> caps = e.getValue();

                JSONArray lo = new JSONArray();
                lo.add(caps.get(0));
                lo.add(caps.get(1));
                JSONArray hi = new JSONArray();
                hi.add(caps.get(2));
                hi.add(caps.get(3));

                JSONObject capEntry = new JSONObject();
                capEntry.put("metaperson-id", e.getKey());
                capEntry.put("lo", lo);
                capEntry.put("hi", hi);

                capacity.add(capEntry);

            }
            o.put("capacity", capacity);

            data.add(o);
        }

        Model.getInstance().writeJSON(fname, data, "metaevents");

    }

}
