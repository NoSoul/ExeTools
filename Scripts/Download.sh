# !/bin/sh
if [ $# -ne 2 ]; then
    echo "Usage: $0 MaxDownloadLimit FilePath"
    exit 1;
fi
aria2c -x3 --max-overall-upload-limit=10K --max-overall-download-limit=$1K $2
exit 0;
