const webpack = require('webpack');
const path = require('path');
const { CleanWebpackPlugin } = require('clean-webpack-plugin');

module.exports = {
    entry: {
        lib: "./sources-alpha/pagescripts/lib.js",
        book: "./sources-alpha/pagescripts/book.js",
        login: "./sources-alpha/pagescripts/login.js",
        settings: "./sources-alpha/pagescripts/settings.js",
        users: "./sources-alpha/pagescripts/users.js",
        fzf: "./sources-alpha/pagescripts/404.js"
    },
    output: {
        path: path.join(__dirname, './public-alpha/src/'),
        filename: "./[name].js"
    },
    module: {
        rules: [
            {
                test: /\.css$/,
                use: [
                    {
                        loader: 'style-loader'
                    },
                    {
                        loader: 'css-loader'
                    }
                ]
            },
            {
                test: /\.less$/,
                exclude: /node_modules/,
                use: [
                    {
                        loader: 'style-loader'
                    },
                    {
                        loader: 'css-loader'
                    },
                    {
                        loader: 'less-loader'
                    }
                    ]
            },
            {
                test: /\.js$/,
                exclude: /(node_modules|bower_components)/,
                use: {
                    loader: "babel-loader",
                    options: {
                        presets: ["@babel/preset-env"]
                    }
                }
            },
            {
                test: /\.(png|jpe?g|ico)$/i,
                loader: 'file-loader'
            }
        ]
    },
    plugins: [
        new CleanWebpackPlugin(),
        new webpack.ProvidePlugin({
            $: 'jquery',
            jQuery: 'jquery'
        })
    ]
}
