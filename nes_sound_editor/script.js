// NES Sound Editor - Main Script

// Constants for NES sound channels
const CHANNELS = {
    PULSE1: 0,
    PULSE2: 1,
    TRIANGLE: 2,
    NOISE: 3,
    DMC: 4
};

// NES note frequencies (in Hz)
const NES_NOTES = {
    'C-1': 44, 'C#1': 47, 'D-1': 50, 'D#1': 53, 'E-1': 56, 'F-1': 59, 'F#1': 63, 'G-1': 67, 'G#1': 71, 'A-1': 75, 'A#1': 79, 'B-1': 84,
    'C-2': 89, 'C#2': 94, 'D-2': 100, 'D#2': 106, 'E-2': 112, 'F-2': 119, 'F#2': 126, 'G-2': 134, 'G#2': 142, 'A-2': 150, 'A#2': 159, 'B-2': 169,
    'C-3': 179, 'C#3': 189, 'D-3': 200, 'D#3': 212, 'E-3': 225, 'F-3': 238, 'F#3': 252, 'G-3': 267, 'G#3': 283, 'A-3': 300, 'A#3': 318, 'B-3': 337,
    'C-4': 357, 'C#4': 378, 'D-4': 401, 'D#4': 425, 'E-4': 450, 'F-4': 477, 'F#4': 505, 'G-4': 535, 'G#4': 567, 'A-4': 601, 'A#4': 637, 'B-4': 674,
    'C-5': 714, 'C#5': 757, 'D-5': 802, 'D#5': 850, 'E-5': 900, 'F-5': 954, 'F#5': 1010, 'G-5': 1070, 'G#5': 1134, 'A-5': 1202, 'A#5': 1273, 'B-5': 1349,
    'C-6': 1429, 'C#6': 1514, 'D-6': 1604, 'D#6': 1699, 'E-6': 1800, 'F-6': 1907, 'F#6': 2020, 'G-6': 2140, 'G#6': 2268, 'A-6': 2403, 'A#6': 2546, 'B-6': 2697,
    'C-7': 2857, 'C#7': 3027, 'D-7': 3207, 'D#7': 3398, 'E-7': 3600, 'F-7': 3814, 'F#7': 4040, 'G-7': 4281, 'G#7': 4535, 'A-7': 4806, 'A#7': 5092, 'B-7': 5394,
    'C-8': 5714
};

// NES note periods (for APU)
const NES_PERIODS = {
    'C-1': 428, 'C#1': 404, 'D-1': 381, 'D#1': 360, 'E-1': 339, 'F-1': 320, 'F#1': 302, 'G-1': 285, 'G#1': 269, 'A-1': 254, 'A#1': 240, 'B-1': 226,
    'C-2': 214, 'C#2': 202, 'D-2': 190, 'D#2': 180, 'E-2': 170, 'F-2': 160, 'F#2': 151, 'G-2': 143, 'G#2': 135, 'A-2': 127, 'A#2': 120, 'B-2': 113,
    'C-3': 107, 'C#3': 101, 'D-3': 95, 'D#3': 90, 'E-3': 85, 'F-3': 80, 'F#3': 76, 'G-3': 71, 'G#3': 67, 'A-3': 64, 'A#3': 60, 'B-3': 57,
    'C-4': 54, 'C#4': 51, 'D-4': 48, 'D#4': 45, 'E-4': 43, 'F-4': 40, 'F#4': 38, 'G-4': 36, 'G#4': 34, 'A-4': 32, 'A#4': 30, 'B-4': 28,
    'C-5': 27, 'C#5': 25, 'D-5': 24, 'D#5': 23, 'E-5': 21, 'F-5': 20, 'F#5': 19, 'G-5': 18, 'G#5': 17, 'A-5': 16, 'A#5': 15, 'B-5': 14,
    'C-6': 13, 'C#6': 13, 'D-6': 12, 'D#6': 11, 'E-6': 11, 'F-6': 10, 'F#6': 10, 'G-6': 9, 'G#6': 8, 'A-6': 8, 'A#6': 8, 'B-6': 7,
    'C-7': 7, 'C#7': 6, 'D-7': 6, 'D#7': 6, 'E-7': 5, 'F-7': 5, 'F#7': 5, 'G-7': 5, 'G#7': 4, 'A-7': 4, 'A#7': 4, 'B-7': 4,
    'C-8': 3
};

// NES note lengths (in frames)
const NOTE_LENGTHS = {
    '1': 64,  // Whole note
    '2': 32,  // Half note
    '4': 16,  // Quarter note
    '8': 8,   // Eighth note
    '16': 4,  // Sixteenth note
    '32': 2,  // Thirty-second note
    '64': 1   // Sixty-fourth note
};

// NES duty cycles
const DUTY_CYCLES = {
    0: 0.125, // 12.5%
    1: 0.25,  // 25%
    2: 0.5,   // 50%
    3: 0.75   // 75%
};

// Class to represent a note in the NES sound editor
class Note {
    constructor(pitch, startTime, length, velocity = 15, instrument = 0) {
        this.pitch = pitch;         // Note name (e.g., 'C-4')
        this.startTime = startTime; // Start time in frames
        this.length = length;       // Length in frames
        this.velocity = velocity;   // Volume (0-15)
        this.instrument = instrument; // Instrument number
    }

    // Convert to NES .s format
    toNESFormat() {
        const period = NES_PERIODS[this.pitch];
        const periodLow = period & 0xFF;
        const periodHigh = (period >> 8) & 0x07;
        
        // Format: [note length][period low][period high][instrument][volume]
        return [
            this.length,
            periodLow,
            periodHigh,
            this.instrument,
            this.velocity
        ];
    }
}

// Class to represent a track in the NES sound editor
class Track {
    constructor(channel) {
        this.channel = channel;
        this.notes = [];
        this.tempo = 120; // BPM
        this.name = "New Track";
    }

    // Add a note to the track
    addNote(note) {
        this.notes.push(note);
        this.notes.sort((a, b) => a.startTime - b.startTime);
    }

    // Remove a note from the track
    removeNote(index) {
        this.notes.splice(index, 1);
    }

    // Convert to NES .s format
    toNESFormat() {
        const data = [];
        
        // Add track header
        data.push(this.channel); // Channel number
        
        // Add notes
        for (const note of this.notes) {
            data.push(...note.toNESFormat());
        }
        
        // Add end marker
        data.push(0);
        
        return data;
    }
}

// Class to represent a song in the NES sound editor
class Song {
    constructor() {
        this.name = "New Song";
        this.author = "Unknown";
        this.tracks = [
            new Track(CHANNELS.PULSE1),
            new Track(CHANNELS.PULSE2),
            new Track(CHANNELS.TRIANGLE),
            new Track(CHANNELS.NOISE),
            new Track(CHANNELS.DMC)
        ];
    }

    // Convert to NES .s format
    toNESFormat() {
        const data = [];
        
        // Add song header
        data.push(0x4E, 0x45, 0x53, 0x4D); // "NESM" magic
        data.push(0x1A); // EOF marker
        data.push(0x01); // Version
        data.push(this.tracks.length); // Number of tracks
        
        // Add song metadata
        const nameBytes = new TextEncoder().encode(this.name.padEnd(32, '\0'));
        data.push(...nameBytes);
        
        const authorBytes = new TextEncoder().encode(this.author.padEnd(32, '\0'));
        data.push(...authorBytes);
        
        // Add tracks
        for (const track of this.tracks) {
            data.push(...track.toNESFormat());
        }
        
        return data;
    }
}

// Main application class
class NESSoundEditor {
    constructor() {
        this.song = new Song();
        this.currentTrack = 0;
        this.isPlaying = false;
        this.audioContext = null;
        this.oscillators = {};
        
        this.initUI();
        this.setupEventListeners();
    }
    
    // Initialize the UI
    initUI() {
        // Initialize piano roll
        this.initPianoRoll();
        
        // Initialize tempo display
        this.updateTempoDisplay();
        
        // Initialize instrument controls
        this.updateInstrumentControls();
    }
    
    // Initialize the piano roll
    initPianoRoll() {
        const noteLabels = document.querySelector('.note-labels');
        const pianoKeys = document.querySelector('.piano-keys');
        const timeMarkers = document.querySelector('.time-markers');
        const noteGrid = document.querySelector('.note-grid');
        
        // Create note labels
        const notes = Object.keys(NES_NOTES).reverse();
        notes.forEach(note => {
            const label = document.createElement('div');
            label.className = 'note-label';
            label.textContent = note;
            label.style.height = '20px';
            label.style.borderBottom = '1px solid var(--border-color)';
            noteLabels.appendChild(label);
            
            const key = document.createElement('div');
            key.className = 'piano-key';
            key.dataset.note = note;
            key.style.height = '20px';
            key.style.borderBottom = '1px solid var(--border-color)';
            key.style.backgroundColor = note.includes('#') ? '#000' : '#fff';
            key.style.color = note.includes('#') ? '#fff' : '#000';
            pianoKeys.appendChild(key);
        });
        
        // Create time markers
        for (let i = 0; i < 64; i++) {
            const marker = document.createElement('div');
            marker.className = 'time-marker';
            marker.textContent = i;
            marker.style.width = '20px';
            marker.style.textAlign = 'center';
            marker.style.borderRight = '1px solid var(--border-color)';
            timeMarkers.appendChild(marker);
        }
        
        // Make the note grid interactive
        noteGrid.addEventListener('mousedown', this.handleNoteGridClick.bind(this));
    }
    
    // Handle clicks on the note grid
    handleNoteGridClick(event) {
        if (this.isPlaying) return;
        
        const rect = event.target.getBoundingClientRect();
        const x = event.clientX - rect.left;
        const y = event.clientY - rect.top;
        
        const timeIndex = Math.floor(x / 20);
        const noteIndex = Math.floor(y / 20);
        
        const notes = Object.keys(NES_NOTES).reverse();
        if (noteIndex >= 0 && noteIndex < notes.length) {
            const note = notes[noteIndex];
            
            // Check if there's already a note at this position
            const existingNoteIndex = this.song.tracks[this.currentTrack].notes.findIndex(n => 
                n.startTime === timeIndex && n.pitch === note
            );
            
            if (existingNoteIndex >= 0) {
                // Remove the note
                this.song.tracks[this.currentTrack].removeNote(existingNoteIndex);
            } else {
                // Add a new note
                const newNote = new Note(note, timeIndex, 4, 15, parseInt(document.getElementById('instrument-select').value));
                this.song.tracks[this.currentTrack].addNote(newNote);
            }
            
            this.renderNotes();
        }
    }
    
    // Render notes on the piano roll
    renderNotes() {
        const noteGrid = document.querySelector('.note-grid');
        
        // Clear existing notes
        const existingNotes = noteGrid.querySelectorAll('.note');
        existingNotes.forEach(note => note.remove());
        
        // Add notes from the current track
        const track = this.song.tracks[this.currentTrack];
        const notes = Object.keys(NES_NOTES).reverse();
        
        track.notes.forEach(note => {
            const noteElement = document.createElement('div');
            noteElement.className = 'note';
            noteElement.style.position = 'absolute';
            noteElement.style.left = `${note.startTime * 20}px`;
            noteElement.style.top = `${notes.indexOf(note.pitch) * 20}px`;
            noteElement.style.width = `${note.length * 20}px`;
            noteElement.style.height = '20px';
            noteElement.style.backgroundColor = 'var(--primary-color)';
            noteElement.style.opacity = '0.7';
            noteElement.style.borderRadius = '2px';
            
            noteGrid.appendChild(noteElement);
        });
    }
    
    // Update the tempo display
    updateTempoDisplay() {
        const tempoValue = document.getElementById('tempo-value');
        tempoValue.textContent = this.song.tracks[this.currentTrack].tempo;
    }
    
    // Update the instrument controls
    updateInstrumentControls() {
        const instrumentSelect = document.getElementById('instrument-select');
        const dutyCycleSelect = document.getElementById('duty-cycle');
        const volumeSlider = document.getElementById('volume');
        const sweepSlider = document.getElementById('sweep');
        
        // Set default values
        dutyCycleSelect.value = 2; // 50% duty cycle
        volumeSlider.value = 12;   // 75% volume
        sweepSlider.value = 0;     // No sweep
    }
    
    // Set up event listeners
    setupEventListeners() {
        // Play button
        document.getElementById('play-btn').addEventListener('click', () => this.play());
        
        // Stop button
        document.getElementById('stop-btn').addEventListener('click', () => this.stop());
        
        // Save button
        document.getElementById('save-btn').addEventListener('click', () => this.save());
        
        // Load button
        document.getElementById('load-btn').addEventListener('click', () => {
            document.getElementById('file-input').click();
        });
        
        // File input
        document.getElementById('file-input').addEventListener('change', (event) => this.load(event));
        
        // Track selector
        document.getElementById('track-select').addEventListener('change', (event) => {
            this.currentTrack = parseInt(event.target.value);
            this.renderNotes();
        });
        
        // Tempo slider
        document.getElementById('tempo-slider').addEventListener('input', (event) => {
            this.song.tracks[this.currentTrack].tempo = parseInt(event.target.value);
            this.updateTempoDisplay();
        });
        
        // Song name and author
        document.getElementById('song-name').addEventListener('input', (event) => {
            this.song.name = event.target.value;
        });
        
        document.getElementById('song-author').addEventListener('input', (event) => {
            this.song.author = event.target.value;
        });
    }
    
    // Play the current song
    play() {
        if (this.isPlaying) return;
        
        this.isPlaying = true;
        
        // Create audio context if it doesn't exist
        if (!this.audioContext) {
            this.audioContext = new (window.AudioContext || window.webkitAudioContext)();
        }
        
        // Play each track
        this.song.tracks.forEach((track, trackIndex) => {
            if (track.notes.length === 0) return;
            
            // Create an oscillator for this track
            const oscillator = this.audioContext.createOscillator();
            const gainNode = this.audioContext.createGain();
            
            oscillator.connect(gainNode);
            gainNode.connect(this.audioContext.destination);
            
            // Set oscillator type based on track
            switch (track.channel) {
                case CHANNELS.PULSE1:
                case CHANNELS.PULSE2:
                    oscillator.type = 'square';
                    break;
                case CHANNELS.TRIANGLE:
                    oscillator.type = 'triangle';
                    break;
                case CHANNELS.NOISE:
                    oscillator.type = 'sawtooth';
                    break;
                case CHANNELS.DMC:
                    oscillator.type = 'sine';
                    break;
            }
            
            // Start the oscillator
            oscillator.start();
            
            // Schedule notes
            const startTime = this.audioContext.currentTime;
            const secondsPerBeat = 60 / track.tempo;
            
            track.notes.forEach(note => {
                const noteStartTime = startTime + (note.startTime * secondsPerBeat / 4);
                const noteEndTime = noteStartTime + (note.length * secondsPerBeat / 4);
                
                // Set frequency
                oscillator.frequency.setValueAtTime(NES_NOTES[note.pitch], noteStartTime);
                
                // Set volume
                gainNode.gain.setValueAtTime(note.velocity / 15, noteStartTime);
                gainNode.gain.setValueAtTime(0, noteEndTime);
            });
            
            // Stop the oscillator after the last note
            const lastNote = track.notes[track.notes.length - 1];
            const endTime = startTime + ((lastNote.startTime + lastNote.length) * secondsPerBeat / 4);
            oscillator.stop(endTime);
            
            // Store the oscillator
            this.oscillators[trackIndex] = oscillator;
        });
        
        // Update UI
        document.getElementById('play-btn').textContent = 'Pause';
        document.getElementById('play-btn').removeEventListener('click', this.play);
        document.getElementById('play-btn').addEventListener('click', () => this.pause());
    }
    
    // Pause the current song
    pause() {
        if (!this.isPlaying) return;
        
        this.isPlaying = false;
        
        // Stop all oscillators
        Object.values(this.oscillators).forEach(oscillator => {
            oscillator.stop();
        });
        
        this.oscillators = {};
        
        // Update UI
        document.getElementById('play-btn').textContent = 'Play';
        document.getElementById('play-btn').removeEventListener('click', this.pause);
        document.getElementById('play-btn').addEventListener('click', () => this.play());
    }
    
    // Stop the current song
    stop() {
        this.pause();
        
        // Reset audio context
        if (this.audioContext) {
            this.audioContext.close();
            this.audioContext = null;
        }
    }
    
    // Save the current song as a .s file
    save() {
        const data = this.song.toNESFormat();
        const blob = new Blob([new Uint8Array(data)], { type: 'application/octet-stream' });
        const url = URL.createObjectURL(blob);
        
        const a = document.createElement('a');
        a.href = url;
        a.download = `${this.song.name}.s`;
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        URL.revokeObjectURL(url);
    }
    
    // Load a song from a .s file
    load(event) {
        const file = event.target.files[0];
        if (!file) return;
        
        const reader = new FileReader();
        reader.onload = (e) => {
            const data = new Uint8Array(e.target.result);
            
            // Check if it's a valid NES .s file
            if (data[0] !== 0x4E || data[1] !== 0x45 || data[2] !== 0x53 || data[3] !== 0x4D) {
                alert('Invalid NES .s file');
                return;
            }
            
            // TODO: Parse the .s file and load it into the editor
            // This is a placeholder for now
            alert('Loading .s files is not implemented yet');
        };
        
        reader.readAsArrayBuffer(file);
    }
}

// Initialize the editor when the page loads
window.addEventListener('DOMContentLoaded', () => {
    const editor = new NESSoundEditor();
}); 