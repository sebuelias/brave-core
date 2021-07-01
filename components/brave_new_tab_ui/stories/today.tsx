// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

import * as React from 'react'
import { withKnobs, text } from '@storybook/addon-knobs'
import ThemeProvider from '../../common/StorybookThemeProvider'
import BraveTodayLoadingCard from '../components/default/braveToday/cards/cardLoading'
import BraveTodayErrorCard from '../components/default/braveToday/cards/cardError'
import PublisherMeta from '../components/default/braveToday/cards/PublisherMeta'
import DisplayAdCard from '../components/default/braveToday/cards/displayAd'
import getBraveNewsDisplayAd from './default/data/getBraveNewsDisplayAd'

const onClick = (() => alert('clicked'))

export default {
  title: 'New Tab/Brave Today',
  decorators: [
    (Story: any) => <ThemeProvider><Story /></ThemeProvider>,
    (Story: any) => (
      <div
        style={{
          display: 'flex',
          fontFamily: 'Poppins',
          alignItems: 'center',
          justifyContent: 'center',
          gap: '20px',
          width: '100%',
          minHeight: '100vh',
          background: 'url(https://placekitten.com/2000/3000)',
          backgroundSize: 'contain',
          color: 'pink',
          fontSize: '22px',
          fontWeight: 600
        }}
      >
        <Story />
      </div>
    ),
    withKnobs
  ]
}

export const Publisher = () => (
  <>
    <PublisherMeta
      publisher={{
        publisher_id: '123abc',
        publisher_name: text('Publisher Name', 'small'),
        category: 'Top News',
        enabled: true,
        user_enabled: null
      }}
      onSetPublisherPref={onClick}
    />
    <PublisherMeta
      publisher={{
        publisher_id: '123abcdef',
        publisher_name: text('Publisher Name 2', 'The Miller Chronicle'),
        category: 'Top News',
        enabled: true,
        user_enabled: null
      }}
      onSetPublisherPref={onClick}
    />
  </>
)

export const Loading = () => (
  <BraveTodayLoadingCard />
)

export const Error = () => (
  <BraveTodayErrorCard />
)

const handleDisplayAdVisit = () => alert('handle visit')
const handleDisplayAdView = () => console.log('display ad viewed')

export const DisplayAd = () => (
  <DisplayAdCard
    getContent={getBraveNewsDisplayAd.bind(undefined, true)}
    onVisitDisplayAd={handleDisplayAdVisit}
    onViewedDisplayAd={handleDisplayAdView}
  />
)
