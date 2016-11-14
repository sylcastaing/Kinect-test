{
    'targets': [
        {
            'target_name': 'kinect',
            'sources': [
                'src/addon.cc',
                'src/kinect.cc'
            ],
            'libraries': [
                '/usr/local/lib/libfreenect.a',
            ]
        }
    ]
}